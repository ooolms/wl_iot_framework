#include "ARpcControlsDefinition.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNamedNodeMap>

static bool parseJsonCommand(const QJsonObject &controlObject,ARpcControlsCommand &control)
{
	if(controlObject["command"].toString().isEmpty())return false;
	control.title=controlObject["title"].toString();
	control.command=controlObject["command"].toString();
	control.syncCall=false;
	if(controlObject.contains("sync")&&controlObject["sync"].isBool()&&controlObject["sync"].toBool())
		control.syncCall=true;
	control.params.clear();
	if(!controlObject.contains("params"))return true;
	if(!controlObject["params"].isArray())return false;
	QJsonArray paramsArray=controlObject["params"].toArray();
	for(int i=0;i<paramsArray.count();++i)
	{
		if(!paramsArray[i].isObject())return false;
		QJsonObject paramObject=paramsArray[i].toObject();
		if(!paramObject.contains("param"))return false;
		ARpcControlParam p;
		p.title=paramObject["param"].toString();
		p.type=ARpcControlParam::typeFromString(paramObject["type"].toString());
		if(p.type==ARpcControlParam::BAD_TYPE)return false;
		if(paramObject.contains("constraints"))
		{
			QJsonObject constr=paramObject["constraints"].toObject();
			for(auto i=constr.begin();i!=constr.end();++i)
			{
				if(i.value().isDouble())
					p.constraints[i.key()]=QString::fromUtf8(QByteArray::number(i.value().toDouble()));
				else p.constraints[i.key()]=i.value().toString();
			}
		}
		control.params.append(p);
	}
	return true;
}

static bool parseJsonGroup(const QJsonObject &groupObject,ARpcControlsGroup &group)
{
	group.title=groupObject["title"].toString();
	group.layout=ARpcControlsGroup::VERTICAL;
	group.elements.clear();
	if(groupObject.contains("layout")&&groupObject["layout"].toString()=="h")
		group.layout=ARpcControlsGroup::HORIZONTAL;
	if(groupObject.contains("elements")&&groupObject["elements"].isArray())
	{
		QJsonArray elemsArray=groupObject["elements"].toArray();
		for(int i=0;i<elemsArray.count();++i)
		{
			if(!elemsArray[i].isObject())return false;
			QJsonObject obj=elemsArray[i].toObject();
			QString eType=obj["element_type"].toString();
			if(eType=="group")
			{
				ARpcControlsGroup *g=new ARpcControlsGroup;
				if(!parseJsonGroup(obj,*g))
				{
					delete g;
					return false;
				}
				group.elements.append(ARpcControlsGroup::Element(g));
			}
			else if(eType=="control")
			{
				ARpcControlsCommand *c=new ARpcControlsCommand;
				if(!parseJsonCommand(obj,*c))
				{
					delete c;
					return false;
				}
				group.elements.append(ARpcControlsGroup::Element(c));
			}
			else return false;
		}
	}
	return true;
}

static bool parseXmlCommand(QDomElement commandElem,ARpcControlsCommand &command)
{
	if(commandElem.attribute("command").isEmpty())return false;
	command.title=commandElem.attribute("title");
	command.command=commandElem.attribute("command");
	command.syncCall=(commandElem.attribute("sync")!="0");
	command.params.clear();
	for(int i=0;i<commandElem.childNodes().count();++i)
	{
		QDomElement paramElem=commandElem.childNodes().at(i).toElement();
		if(paramElem.isNull())continue;
		if(paramElem.nodeName()!="param")return false;
		ARpcControlParam p;
		p.title=paramElem.attribute("title");
		p.type=ARpcControlParam::typeFromString(paramElem.attribute("type"));
		if(p.type==ARpcControlParam::BAD_TYPE)return false;
		QDomElement constrElem=paramElem.firstChildElement("constraints");
		if(!constrElem.isNull())
		{
			QDomNamedNodeMap attrs=constrElem.attributes();
			for(int i=0;i<attrs.count();++i)
				p.constraints[attrs.item(i).nodeName()]=attrs.item(i).nodeValue();
		}
		command.params.append(p);
	}
	return true;
}

static bool parseXmlGroup(QDomElement groupElem,ARpcControlsGroup &group)
{
	group.title=groupElem.attribute("title");
	group.layout=ARpcControlsGroup::VERTICAL;
	if(groupElem.attribute("layout")=="h")group.layout=ARpcControlsGroup::HORIZONTAL;
	group.elements.clear();
	for(int i=0;i<groupElem.childNodes().count();++i)
	{
		QDomElement elem=groupElem.childNodes().at(i).toElement();
		if(elem.isNull())continue;
		if(elem.nodeName()=="group")
		{
			ARpcControlsGroup *g=new ARpcControlsGroup;
			if(!parseXmlGroup(elem,*g))
			{
				delete g;
				return false;
			}
			group.elements.append(ARpcControlsGroup::Element(g));
		}
		else if(elem.nodeName()=="control")
		{
			ARpcControlsCommand *c=new ARpcControlsCommand;
			if(!parseXmlCommand(elem,*c))
			{
				delete c;
				return false;
			}
			group.elements.append(ARpcControlsGroup::Element(c));
		}
		else return false;
	}
	return true;
}

bool ARpcControlsGroup::parseJsonDescription(const QString &data,ARpcControlsGroup &controls)
{
	//CRIT fix json in doc and parsing here
	QJsonDocument doc=QJsonDocument::fromJson(data.toUtf8());
	if(!doc.isObject())return false;
	if(!doc.object().contains("controls"))return false;
	if(!doc.object()["controls"].isObject())return false;
	QJsonObject rootGroupObject=doc.object()["controls"].toObject();
	if(rootGroupObject["element_type"].toString()!="group")return false;
	return parseJsonGroup(rootGroupObject,controls);
}

bool ARpcControlsGroup::parseXmlDescription(const QString &data,ARpcControlsGroup &controls)
{
	QDomDocument doc;
	if(!doc.setContent(data))return false;
	QDomElement rootElem=doc.firstChildElement("controls");
	if(rootElem.isNull())return false;
	QDomElement groupElem=rootElem.firstChildElement("group");
	if(groupElem.isNull())return false;
	return parseXmlGroup(groupElem,controls);
}

ARpcControlsGroup::Element::Element(ARpcControlsCommand *c)
{
	type=CONTROL;
	value=QSharedPointer<ARpcControlsElement>(c);
}

ARpcControlsGroup::Element::Element(ARpcControlsGroup *g)
{
	type=GROUP;
	value=QSharedPointer<ARpcControlsElement>(g);
}

bool ARpcControlsGroup::Element::isGroup()const
{
	return type==GROUP;
}

bool ARpcControlsGroup::Element::isControl()const
{
	return type==CONTROL;
}

ARpcControlsGroup* ARpcControlsGroup::Element::group()
{
	if(type==GROUP)return static_cast<ARpcControlsGroup*>(value.data());
	return 0;
}

ARpcControlsCommand* ARpcControlsGroup::Element::control()
{
	if(type==CONTROL)return static_cast<ARpcControlsCommand*>(value.data());
	return 0;
}

const ARpcControlsGroup* ARpcControlsGroup::Element::group()const
{
	if(type==GROUP)return static_cast<const ARpcControlsGroup*>(value.data());
	return 0;
}

const ARpcControlsCommand* ARpcControlsGroup::Element::control()const
{
	if(type==CONTROL)return static_cast<const ARpcControlsCommand*>(value.data());
	return 0;
}

QString ARpcControlParam::typeToString(ARpcControlParam::Type t)
{
	if(t==CHECKBOX)return "checkbox";
	else if(t==TEXT_EDIT)return "text_edit";
	else if(t==SELECT)return "select";
	else if(t==SLIDER)return "slider";
	else if(t==DIAL)return "dial";
	else return QString();
}

ARpcControlParam::Type ARpcControlParam::typeFromString(const QString &s)
{
	if(s=="checkbox")return CHECKBOX;
	else if(s=="text_edit")return TEXT_EDIT;
	else if(s=="select")return SELECT;
	else if(s=="slider")return SLIDER;
	else if(s=="dial")return DIAL;
	else return BAD_TYPE;
}
