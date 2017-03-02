#include "ARpcControlsDefinition.h"
#include "ARpcCommonRc.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNamedNodeMap>
#include <QFile>
#include <QXmlSchema>
#include <QXmlSchemaValidator>

static bool parseJsonCommand(const QJsonObject &controlObject,ARpcCommandControl &control)
{
	if(controlObject["command"].toString().isEmpty())return false;
	control.title=controlObject["title"].toString();
	control.command=controlObject["command"].toString();
	control.syncCall=true;
	if(controlObject.contains("sync")&&controlObject["sync"].isBool()&&!controlObject["sync"].toBool())
		control.syncCall=false;
	control.layout=Qt::Vertical;
	if(controlObject.contains("layout")&&controlObject["layout"].toString()=="h")
		control.layout=Qt::Horizontal;
	control.params.clear();
	if(!controlObject.contains("params"))return true;
	if(!controlObject["params"].isArray())return false;
	QJsonArray paramsArray=controlObject["params"].toArray();
	for(int i=0;i<paramsArray.count();++i)
	{
		if(!paramsArray[i].isObject())return false;
		QJsonObject paramObject=paramsArray[i].toObject();
		if(!paramObject.contains("title"))return false;
		ARpcControlParam p;
		p.title=paramObject["title"].toString();
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
	group.layout=Qt::Vertical;
	if(groupObject.contains("layout")&&groupObject["layout"].toString()=="h")
		group.layout=Qt::Horizontal;
	group.elements.clear();
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
				ARpcCommandControl *c=new ARpcCommandControl;
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

static bool parseXmlCommand(QDomElement commandElem,ARpcCommandControl &command)
{
	if(commandElem.attribute("command").isEmpty())return false;
	command.title=commandElem.attribute("title");
	command.command=commandElem.attribute("command");
	command.syncCall=(commandElem.attribute("sync")!="0");
	command.layout=((commandElem.attribute("layout")=="h")?Qt::Horizontal:Qt::Vertical);
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
	group.layout=((groupElem.attribute("layout")=="h")?Qt::Horizontal:Qt::Vertical);
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
			ARpcCommandControl *c=new ARpcCommandControl;
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

static void dumpControlToJson(QJsonObject &controlObj,const ARpcCommandControl &c)
{
	controlObj["element_type"]="control";
	controlObj["title"]=c.title;
	controlObj["command"]=c.command;
	if(c.layout==Qt::Horizontal)controlObj["layout"]="h";
	if(!c.syncCall)controlObj["sync"]=QJsonValue(false);
	if(!c.params.isEmpty())
	{
		QJsonArray paramsArray;
		for(const ARpcControlParam &p:c.params)
		{
			QJsonObject paramObj;
			paramObj["title"]=p.title;
			paramObj["type"]=ARpcControlParam::typeToString(p.type);
			if(!p.constraints.isEmpty())
			{
				QJsonObject constraintsObj;
				for(auto i=p.constraints.begin();i!=p.constraints.end();++i)
					constraintsObj[i.key()]=i.value();
				paramObj["constraints"]=constraintsObj;
			}
			paramsArray.append(paramObj);
		}
		controlObj["params"]=paramsArray;
	}
}

static void dumpGroupToJson(QJsonObject &groupObj,const ARpcControlsGroup &g)
{
	groupObj["element_type"]="group";
	groupObj["title"]=g.title;
	if(g.layout==Qt::Horizontal)groupObj["layout"]="h";
	if(!g.elements.isEmpty())
	{
		QJsonArray elementsArray;
		for(const ARpcControlsGroup::Element &e:g.elements)
		{
			if(e.isGroup())
			{
				QJsonObject childObj;
				dumpGroupToJson(childObj,*e.group());
				elementsArray.append(childObj);
			}
			else if(e.isControl())
			{
				QJsonObject childObj;
				dumpControlToJson(childObj,*e.control());
				elementsArray.append(childObj);
			}
		}
		groupObj["elements"]=elementsArray;
	}
}

static void dumpControlToXml(QDomDocument &doc,QDomElement &controlElem,const ARpcCommandControl &c)
{
	controlElem.setAttribute("title",c.title);
	controlElem.setAttribute("command",c.command);
	if(c.layout==Qt::Horizontal)controlElem.setAttribute("layout","h");
	if(!c.syncCall)controlElem.setAttribute("sync","0");
	for(const ARpcControlParam &p:c.params)
	{
		QDomElement paramElem=doc.createElement("param");
		controlElem.appendChild(paramElem);
		paramElem.setAttribute("title",p.title);
		paramElem.setAttribute("type",ARpcControlParam::typeToString(p.type));
		if(!p.constraints.isEmpty())
		{
			QDomElement constraintsElem=doc.createElement("constraints");
			paramElem.appendChild(constraintsElem);
			for(auto i=p.constraints.begin();i!=p.constraints.end();++i)
				constraintsElem.setAttribute(i.key(),i.value());
		}
	}
}

static void dumpGroupToXml(QDomDocument &doc,QDomElement &groupElem,const ARpcControlsGroup &grp)
{
	groupElem.setAttribute("title",grp.title);
	if(grp.layout==Qt::Horizontal)groupElem.setAttribute("layout","h");
	for(const ARpcControlsGroup::Element &elem:grp.elements)
	{
		if(elem.isGroup())
		{
			QDomElement childElem=doc.createElement("group");
			groupElem.appendChild(childElem);
			dumpGroupToXml(doc,childElem,*elem.group());
		}
		else if(elem.isControl())
		{
			QDomElement childElem=doc.createElement("control");
			groupElem.appendChild(childElem);
			dumpControlToXml(doc,childElem,*elem.control());
		}
	}
}

bool ARpcControlsGroup::parseJsonDescription(const QString &data,ARpcControlsGroup &controls)
{
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
	ARpcCommonRc::initRc();
	QXmlSchema schema;
	QFile file(":/ARpc/controls.xsd");
	file.open(QIODevice::ReadOnly);
	if(schema.load(&file))
	{
		file.close();
		QXmlSchemaValidator validator(schema);
		if(!validator.validate(data.toUtf8()))
			return false;
	}
	else file.close();
	QDomDocument doc;
	if(!doc.setContent(data))return false;
	QDomElement rootElem=doc.firstChildElement("controls");
	if(rootElem.isNull())return false;
	QDomElement groupElem=rootElem.firstChildElement("group");
	if(groupElem.isNull())return false;
	return parseXmlGroup(groupElem,controls);
}

void ARpcControlsGroup::dumpToJson(QString &data,const ARpcControlsGroup &controls)
{
	QJsonObject rootObj;
	QJsonObject groupObj;
	dumpGroupToJson(groupObj,controls);
	rootObj["controls"]=groupObj;
	QJsonDocument doc;
	doc.setObject(rootObj);
	data=QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
}

void ARpcControlsGroup::dumpToXml(QString &data,const ARpcControlsGroup &controls)
{
	QDomDocument doc;
	QDomElement rootElem=doc.createElement("controls");
	doc.appendChild(rootElem);
	QDomElement groupElem=doc.createElement("group");
	rootElem.appendChild(groupElem);
	dumpGroupToXml(doc,groupElem,controls);
	data=doc.toString(-1);
}

ARpcControlsGroup::Element::Element(ARpcCommandControl *c)
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

ARpcCommandControl* ARpcControlsGroup::Element::control()
{
	if(type==CONTROL)return static_cast<ARpcCommandControl*>(value.data());
	return 0;
}

const ARpcControlsGroup* ARpcControlsGroup::Element::group()const
{
	if(type==GROUP)return static_cast<const ARpcControlsGroup*>(value.data());
	return 0;
}

const ARpcCommandControl* ARpcControlsGroup::Element::control()const
{
	if(type==CONTROL)return static_cast<const ARpcCommandControl*>(value.data());
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
