/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

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

static bool parseJsonCommand(const QJsonObject &controlObject,ARpcCommandControl &control,bool shortStrings)
{
	if(controlObject[shortStrings?"c":"command"].toString().isEmpty())return false;
	control.title=controlObject[shortStrings?"t":"title"].toString().toUtf8();
	control.command=controlObject[shortStrings?"c":"command"].toString().toUtf8();
	control.layout=Qt::Vertical;
	if(controlObject.contains(shortStrings?"l":"layout")&&controlObject[shortStrings?"l":"layout"].toString()=="h")
		control.layout=Qt::Horizontal;
	control.params.clear();
	if(!controlObject.contains(shortStrings?"ps":"params"))return true;
	if(!controlObject[shortStrings?"ps":"params"].isArray())return false;
	QJsonArray paramsArray=controlObject["params"].toArray();
	for(int i=0;i<paramsArray.count();++i)
	{
		if(!paramsArray[i].isObject())return false;
		QJsonObject paramObject=paramsArray[i].toObject();
		if(!paramObject.contains(shortStrings?"tl":"title"))return false;
		ARpcControlParam p;
		p.title=paramObject[shortStrings?"tl":"title"].toString().toUtf8();
		p.type=ARpcControlParam::typeFromString(paramObject[shortStrings?"t":"type"].toString().toUtf8());
		if(p.type==ARpcControlParam::BAD_TYPE)return false;
		if(paramObject.contains(shortStrings?"cs":"constraints"))
		{
			QJsonObject constr=paramObject[shortStrings?"cs":"constraints"].toObject();
			for(auto i=constr.begin();i!=constr.end();++i)
			{
				if(i.value().isDouble())
					p.constraints[i.key().toUtf8()]=QByteArray::number(i.value().toDouble());
				else p.constraints[i.key().toUtf8()]=i.value().toString().toUtf8();
			}
		}
		control.params.append(p);
	}
	return true;
}

static bool parseJsonGroup(const QJsonObject &groupObject,ARpcControlsGroup &group,bool shortStrings)
{
	group.title=groupObject[shortStrings?"t":"title"].toString().toUtf8();
	group.layout=Qt::Vertical;
	if(groupObject.contains(shortStrings?"l":"layout")&&groupObject[shortStrings?"l":"layout"].toString()=="h")
		group.layout=Qt::Horizontal;
	group.elements.clear();
	if(groupObject.contains(shortStrings?"els":"elements")&&groupObject[shortStrings?"els":"elements"].isArray())
	{
		QJsonArray elemsArray=groupObject[shortStrings?"els":"elements"].toArray();
		for(int i=0;i<elemsArray.count();++i)
		{
			if(!elemsArray[i].isObject())return false;
			QJsonObject obj=elemsArray[i].toObject();
			QString eType=obj[shortStrings?"et":"element_type"].toString();
			if(eType==(shortStrings?"g":"group"))
			{
				ARpcControlsGroup *g=new ARpcControlsGroup;
				if(!parseJsonGroup(obj,*g,shortStrings))
				{
					delete g;
					return false;
				}
				group.elements.append(ARpcControlsGroup::Element(g));
			}
			else if(eType==(shortStrings?"c":"control"))
			{
				ARpcCommandControl *c=new ARpcCommandControl;
				if(!parseJsonCommand(obj,*c,shortStrings))
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

static bool parseXmlCommand(QDomElement commandElem,ARpcCommandControl &command,bool shortStrings)
{
	QString cmd=commandElem.attribute(shortStrings?"c":"command");
	if(cmd.isEmpty())return false;
	command.title=commandElem.attribute(shortStrings?"t":"title").toUtf8();
	command.command=cmd.toUtf8();
	QString lay=commandElem.attribute(shortStrings?"l":"layout");
	command.layout=((lay=="h")?Qt::Horizontal:Qt::Vertical);
	command.params.clear();
	for(int i=0;i<commandElem.childNodes().count();++i)
	{
		QDomElement paramElem=commandElem.childNodes().at(i).toElement();
		if(paramElem.isNull())continue;
		if(shortStrings)
		{
			if(paramElem.nodeName()!="p")return false;
		}
		else if(paramElem.nodeName()!="param")return false;
		ARpcControlParam p;
		p.title=paramElem.attribute(shortStrings?"tl":"title").toUtf8();
		QByteArray typeStr=paramElem.attribute(shortStrings?"t":"type").toUtf8();
		p.type=ARpcControlParam::typeFromString(typeStr);
		if(p.type==ARpcControlParam::BAD_TYPE)return false;
		QDomElement constrElem=paramElem.firstChildElement(shortStrings?"cs":"constraints");
		if(!constrElem.isNull())
		{
			QDomNamedNodeMap attrs=constrElem.attributes();
			for(int i=0;i<attrs.count();++i)
				p.constraints[attrs.item(i).nodeName().toUtf8()]=attrs.item(i).nodeValue().toUtf8();
		}
		command.params.append(p);
	}
	return true;
}

static bool parseXmlGroup(QDomElement groupElem,ARpcControlsGroup &group,bool shortStrings)
{
	group.title=groupElem.attribute(shortStrings?"t":"title").toUtf8();
	QString lay=groupElem.attribute(shortStrings?"l":"layout");
	group.layout=((lay=="h")?Qt::Horizontal:Qt::Vertical);
	group.elements.clear();
	for(int i=0;i<groupElem.childNodes().count();++i)
	{
		QDomElement elem=groupElem.childNodes().at(i).toElement();
		if(elem.isNull())continue;
		if(elem.nodeName()==(shortStrings?"g":"group"))
		{
			ARpcControlsGroup *g=new ARpcControlsGroup;
			if(!parseXmlGroup(elem,*g,shortStrings))
			{
				delete g;
				return false;
			}
			group.elements.append(ARpcControlsGroup::Element(g));
		}
		else if(elem.nodeName()==(shortStrings?"c":"control"))
		{
			ARpcCommandControl *c=new ARpcCommandControl;
			if(!parseXmlCommand(elem,*c,shortStrings))
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
	controlObj["title"]=QString::fromLocal8Bit(c.title);
	controlObj["command"]=QString::fromUtf8(c.command);
	if(c.layout==Qt::Horizontal)controlObj["layout"]="h";
	if(!c.params.isEmpty())
	{
		QJsonArray paramsArray;
		for(const ARpcControlParam &p:c.params)
		{
			QJsonObject paramObj;
			paramObj["title"]=QString::fromUtf8(p.title);
			paramObj["type"]=QString::fromUtf8(ARpcControlParam::typeToString(p.type));
			if(!p.constraints.isEmpty())
			{
				QJsonObject constraintsObj;
				for(auto i=p.constraints.begin();i!=p.constraints.end();++i)
					constraintsObj[QString::fromUtf8(i.key())]=QString::fromUtf8(i.value());
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
	groupObj["title"]=QString::fromUtf8(g.title);
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
	controlElem.setAttribute("title",QString::fromUtf8(c.title));
	controlElem.setAttribute("command",QString::fromUtf8(c.command));
	if(c.layout==Qt::Horizontal)controlElem.setAttribute("layout","h");
	for(const ARpcControlParam &p:c.params)
	{
		QDomElement paramElem=doc.createElement("param");
		controlElem.appendChild(paramElem);
		paramElem.setAttribute("title",QString::fromUtf8(p.title));
		paramElem.setAttribute("type",QString::fromUtf8(ARpcControlParam::typeToString(p.type)));
		if(!p.constraints.isEmpty())
		{
			QDomElement constraintsElem=doc.createElement("constraints");
			paramElem.appendChild(constraintsElem);
			for(auto i=p.constraints.begin();i!=p.constraints.end();++i)
				constraintsElem.setAttribute(QString::fromUtf8(i.key()),QString::fromUtf8(i.value()));
		}
	}
}

static void dumpGroupToXml(QDomDocument &doc,QDomElement &groupElem,const ARpcControlsGroup &grp)
{
	groupElem.setAttribute("title",QString::fromUtf8(grp.title));
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

bool ARpcControlsGroup::parseJsonDescription(const QByteArray &data,ARpcControlsGroup &controls)
{
	QJsonDocument doc=QJsonDocument::fromJson(data);
	if(!doc.isObject())return false;
	bool shortStrings=false;
	if(!doc.object().contains("controls"))
	{
		if(!doc.object().contains("cls"))
			return false;
		shortStrings=true;
	}
	if(!doc.object()[shortStrings?"cls":"controls"].isObject())return false;
	QJsonObject rootGroupObject=doc.object()[shortStrings?"cls":"controls"].toObject();
	if(rootGroupObject[shortStrings?"et":"element_type"].toString()!=(shortStrings?"g":"group"))return false;
	return parseJsonGroup(rootGroupObject,controls,shortStrings);
}

bool ARpcControlsGroup::parseXmlDescription(const QByteArray &data,ARpcControlsGroup &controls)
{
	ARpcCommonRc::initRc();
//	QXmlSchema schema;
//	QFile file(":/ARpc/controls.xsd");
//	file.open(QIODevice::ReadOnly);
//	if(schema.load(&file))
//	{
//		file.close();
//		QXmlSchemaValidator validator(schema);
//		if(!validator.validate(data))
//			return false;
//	}
//	else file.close();
	QDomDocument doc;
	if(!doc.setContent(data))return false;
	bool shortStrings=false;
	QDomElement rootElem=doc.firstChildElement("controls");
	if(rootElem.isNull())
	{
		rootElem=doc.firstChildElement("cls");
		if(rootElem.isNull())return false;
		shortStrings=true;
	}
	QDomElement groupElem=rootElem.firstChildElement(shortStrings?"g":"group");
	if(groupElem.isNull())return false;
	return parseXmlGroup(groupElem,controls,shortStrings);
}

void ARpcControlsGroup::dumpToJson(QByteArray &data,const ARpcControlsGroup &controls)
{
	QJsonObject rootObj;
	QJsonObject groupObj;
	dumpGroupToJson(groupObj,controls);
	rootObj["controls"]=groupObj;
	QJsonDocument doc;
	doc.setObject(rootObj);
	data=doc.toJson(QJsonDocument::Compact);
}

void ARpcControlsGroup::dumpToXml(QByteArray &data,const ARpcControlsGroup &controls)
{
	QDomDocument doc;
	QDomElement rootElem=doc.createElement("controls");
	doc.appendChild(rootElem);
	QDomElement groupElem=doc.createElement("group");
	rootElem.appendChild(groupElem);
	dumpGroupToXml(doc,groupElem,controls);
	data=doc.toByteArray(-1);
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

QByteArray ARpcControlParam::typeToString(ARpcControlParam::Type t)
{
	if(t==CHECKBOX)return "checkbox";
	else if(t==TEXT_EDIT)return "text_edit";
	else if(t==SELECT)return "select";
	else if(t==SLIDER)return "slider";
	else if(t==DIAL)return "dial";
	else return QByteArray();
}

ARpcControlParam::Type ARpcControlParam::typeFromString(const QByteArray &s)
{
	if(s=="checkbox")return CHECKBOX;
	else if(s=="text_edit")return TEXT_EDIT;
	else if(s=="select")return SELECT;
	else if(s=="slider")return SLIDER;
	else if(s=="dial")return DIAL;
	else return BAD_TYPE;
}
