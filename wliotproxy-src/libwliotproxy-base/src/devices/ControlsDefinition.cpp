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

#include "wliot/devices/ControlsDefinition.h"
#include "wliot/WLIOTCommonRc.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNamedNodeMap>
#include <QFile>
#include <QXmlSchema>
#include <QXmlSchemaValidator>

using namespace WLIOT;

bool ControlsParser::parseJsonCommand(const QJsonObject &controlObject,ControlsCommand &control,
	bool shortStrings,bool ignoreSomeErrors)
{
	if(controlObject[shortStrings?"c":"command"].toString().isEmpty())return false;
	control.title=controlObject[shortStrings?"t":"title"].toString().toUtf8();
	if(controlObject.contains(shortStrings?"bt":"button_text"))
		control.buttonText=controlObject[shortStrings?"bt":"button_text"].toString().toUtf8();
	control.commandToExec=controlObject[shortStrings?"c":"command"].toString().toUtf8();
	if(control.commandToExec.isEmpty()&&!ignoreSomeErrors)return false;
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
		ControlsCommandParam p;
		p.layout=Qt::Vertical;
		if(paramObject.contains(shortStrings?"l":"layout")&&paramObject[shortStrings?"l":"layout"].toString()=="h")
			p.layout=Qt::Horizontal;
		p.title=paramObject[shortStrings?"tl":"title"].toString().toUtf8();
		p.type=ControlsCommandParam::typeFromString(paramObject[shortStrings?"t":"type"].toString().toUtf8());
		if(p.type==ControlsCommandParam::BAD_TYPE)
		{
			if(!ignoreSomeErrors)return false;
			p.type=ControlsCommandParam::HIDDEN;
		}
		if(paramObject.contains(shortStrings?"a":"attributes"))
		{
			QJsonObject attr=paramObject[shortStrings?"a":"attributes"].toObject();
			for(auto i=attr.begin();i!=attr.end();++i)
			{
				if(i.value().isDouble())
					p.attributes[i.key().toUtf8()]=QByteArray::number(i.value().toDouble(),'g',200);
				else p.attributes[i.key().toUtf8()]=i.value().toString().toUtf8();
			}
		}
		control.params.append(p);
	}
	return true;
}

bool ControlsParser::parseJsonGroup(const QJsonObject &groupObject,ControlsGroup &group,
	bool shortStrings,bool ignoreSomeErrors)
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
				ControlsGroup *g=new ControlsGroup;
				if(!parseJsonGroup(obj,*g,shortStrings,ignoreSomeErrors))
				{
					delete g;
					return false;
				}
				group.elements.append(ControlsGroup::Element(g));
			}
			else if(eType==(shortStrings?"c":"control"))
			{
				ControlsCommand *c=new ControlsCommand;
				if(!parseJsonCommand(obj,*c,shortStrings,ignoreSomeErrors))
				{
					delete c;
					return false;
				}
				group.elements.append(ControlsGroup::Element(c));
			}
			else return false;
		}
	}
	return true;
}

bool ControlsParser::parseXmlCommand(QDomElement &commandElem,ControlsCommand &command,
	bool shortStrings,bool ignoreSomeErrors)
{
	QString cmd=commandElem.attribute(shortStrings?"c":"command");
	if(cmd.isEmpty()&&!ignoreSomeErrors)return false;
	command.title=commandElem.attribute(shortStrings?"t":"title").toUtf8();
	command.commandToExec=cmd.toUtf8();
	command.buttonText=commandElem.attribute(shortStrings?"bt":"button_text").toUtf8();
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
		ControlsCommandParam p;
		p.title=paramElem.attribute(shortStrings?"tl":"title").toUtf8();
		QString lay=paramElem.attribute(shortStrings?"l":"layout");
		p.layout=((lay=="h")?Qt::Horizontal:Qt::Vertical);
		QByteArray typeStr=paramElem.attribute(shortStrings?"t":"type").toUtf8();
		p.type=ControlsCommandParam::typeFromString(typeStr);
		if(p.type==ControlsCommandParam::BAD_TYPE)
		{
			if(!ignoreSomeErrors)return false;
			p.type=ControlsCommandParam::HIDDEN;
		}
		QDomElement attrElem=paramElem.firstChildElement(shortStrings?"a":"attributes");
		if(!attrElem.isNull())
		{
			QDomNamedNodeMap xmlAttrs=attrElem.attributes();
			for(int i=0;i<xmlAttrs.count();++i)
				p.attributes[xmlAttrs.item(i).nodeName().toUtf8()]=xmlAttrs.item(i).nodeValue().toUtf8();
		}
		command.params.append(p);
	}
	return true;
}

bool ControlsParser::parseXmlGroup(QDomElement &groupElem,ControlsGroup &group,
	bool shortStrings,bool ignoreSomeErrors)
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
			ControlsGroup *g=new ControlsGroup;
			if(!parseXmlGroup(elem,*g,shortStrings,ignoreSomeErrors))
			{
				delete g;
				return false;
			}
			group.elements.append(ControlsGroup::Element(g));
		}
		else if(elem.nodeName()==(shortStrings?"c":"control"))
		{
			ControlsCommand *c=new ControlsCommand;
			if(!parseXmlCommand(elem,*c,shortStrings,ignoreSomeErrors))
			{
				delete c;
				return false;
			}
			group.elements.append(ControlsGroup::Element(c));
		}
		else return false;
	}
	return true;
}

void ControlsParser::dumpControlToJson(QJsonObject &controlObj,const ControlsCommand &c)
{
	controlObj["element_type"]="control";
	controlObj["title"]=QString::fromLocal8Bit(c.title);
	controlObj["command"]=QString::fromUtf8(c.commandToExec);
	if(!c.buttonText.isEmpty())
		controlObj["button_text"]=QString::fromUtf8(c.buttonText);
	if(c.layout==Qt::Horizontal)controlObj["layout"]="h";
	if(!c.params.isEmpty())
	{
		QJsonArray paramsArray;
		for(const ControlsCommandParam &p:c.params)
		{
			QJsonObject paramObj;
			paramObj["title"]=QString::fromUtf8(p.title);
			paramObj["type"]=QString::fromUtf8(ControlsCommandParam::typeToString(p.type));
			if(p.layout==Qt::Horizontal)
				paramObj["layout"]="h";
			if(!p.attributes.isEmpty())
			{
				QJsonObject attributesObj;
				for(auto i=p.attributes.begin();i!=p.attributes.end();++i)
					attributesObj[QString::fromUtf8(i.key())]=QString::fromUtf8(i.value());
				paramObj["attributes"]=attributesObj;
			}
			paramsArray.append(paramObj);
		}
		controlObj["params"]=paramsArray;
	}
}

void ControlsParser::dumpGroupToJson(QJsonObject &groupObj,const ControlsGroup &g)
{
	groupObj["element_type"]="group";
	groupObj["title"]=QString::fromUtf8(g.title);
	if(g.layout==Qt::Horizontal)groupObj["layout"]="h";
	if(!g.elements.isEmpty())
	{
		QJsonArray elementsArray;
		for(const ControlsGroup::Element &e:g.elements)
		{
			if(e.isGroup())
			{
				QJsonObject childObj;
				dumpGroupToJson(childObj,*e.group());
				elementsArray.append(childObj);
			}
			else if(e.isCommand())
			{
				QJsonObject childObj;
				dumpControlToJson(childObj,*e.command());
				elementsArray.append(childObj);
			}
		}
		groupObj["elements"]=elementsArray;
	}
}

void ControlsParser::dumpControlToXml(QDomDocument &doc,
	QDomElement &controlElem,const ControlsCommand &c,bool shortStrings)
{
	controlElem.setAttribute(shortStrings?"t":"title",QString::fromUtf8(c.title));
	controlElem.setAttribute(shortStrings?"c":"command",QString::fromUtf8(c.commandToExec));
	if(!c.buttonText.isEmpty())
		controlElem.setAttribute(shortStrings?"bt":"button_text",QString::fromUtf8(c.buttonText));
	if(c.layout==Qt::Horizontal)
		controlElem.setAttribute(shortStrings?"l":"layout","h");
	for(const ControlsCommandParam &p:c.params)
	{
		QDomElement paramElem=doc.createElement(shortStrings?"p":"param");
		controlElem.appendChild(paramElem);
		paramElem.setAttribute(shortStrings?"tl":"title",QString::fromUtf8(p.title));
		paramElem.setAttribute(shortStrings?"t":"type",
			QString::fromUtf8(ControlsCommandParam::typeToString(p.type)));
		if(p.layout==Qt::Horizontal)
			paramElem.setAttribute(shortStrings?"l":"layout","h");
		if(!p.attributes.isEmpty())
		{
			QDomElement attributesElem=doc.createElement(shortStrings?"a":"attributes");
			paramElem.appendChild(attributesElem);
			for(auto i=p.attributes.begin();i!=p.attributes.end();++i)
				attributesElem.setAttribute(QString::fromUtf8(i.key()),QString::fromUtf8(i.value()));
		}
	}
}

void ControlsParser::dumpGroupToXml(QDomDocument &doc,QDomElement &groupElem,const ControlsGroup &grp,bool shortStrings)
{
	groupElem.setAttribute(shortStrings?"t":"title",QString::fromUtf8(grp.title));
	if(grp.layout==Qt::Horizontal)
		groupElem.setAttribute(shortStrings?"l":"layout","h");
	for(const ControlsGroup::Element &e:grp.elements)
	{
		if(e.isGroup())
		{
			QDomElement childElem=doc.createElement(shortStrings?"g":"group");
			groupElem.appendChild(childElem);
			dumpGroupToXml(doc,childElem,*e.group(),shortStrings);
		}
		else if(e.isCommand())
		{
			QDomElement childElem=doc.createElement(shortStrings?"c":"control");
			groupElem.appendChild(childElem);
			dumpControlToXml(doc,childElem,*e.command(),shortStrings);
		}
	}
}

void ControlsGroup::extractCommandsList(QList<ControlsCommand> &list)const
{
	for(const ControlsGroup::Element &elem:elements)
	{
		if(elem.isGroup())
			elem.group()->extractCommandsList(list);
		else if(elem.isCommand())
			list.append(*elem.command());
	}
}

void ControlsGroup::extractCommandsMap(QMap<QByteArray,ControlsCommand> &map)const
{
	for(const ControlsGroup::Element &elem:elements)
	{
		if(elem.isGroup())
			elem.group()->extractCommandsMap(map);
		else if(elem.isCommand())
			map[elem.command()->commandToExec]=*elem.command();
	}
}

bool ControlsGroup::operator==(const ControlsGroup &t)const
{
	return layout==t.layout&&title==t.title&&elements==t.elements;
}

bool ControlsParser::parseJsonDescription(const QByteArray &data,ControlsGroup &controls,bool ignoreSomeErrors)
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
	return parseJsonGroup(rootGroupObject,controls,shortStrings,ignoreSomeErrors);
}

bool ControlsParser::parseXmlDescription(const QByteArray &data,ControlsGroup &controls,bool ignoreSomeErrors)
{
	/*WLIOTCommonRc::initRc();
	QXmlSchema schema;
	QFile file(":/wliot/controls.xsd");
	file.open(QIODevice::ReadOnly);
	if(schema.load(&file))
	{
		file.close();
		QXmlSchemaValidator validator(schema);
		if(!validator.validate(data))
		{
			file.setFileName(":/wliot/controls_simplified.xsd");
			file.open(QIODevice::ReadOnly);
			if(schema.load(&file))
			{
				QXmlSchemaValidator validator(schema);
				if(!validator.validate(data))
					return false;
			}
			else file.close();
		}
	}
	else file.close();*/
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
	return parseXmlDescription(rootElem,controls,ignoreSomeErrors,shortStrings);
}

bool ControlsParser::parseXmlDescription(const QDomElement &rootElem,
	ControlsGroup &controls,bool ignoreSomeErrors,bool shortStrings)
{
	QDomElement groupElem=rootElem.firstChildElement(shortStrings?"g":"group");
	if(groupElem.isNull())return false;
	return parseXmlGroup(groupElem,controls,shortStrings,ignoreSomeErrors);
}

void ControlsParser::dumpToJson(QByteArray &data,const ControlsGroup &controls)
{
	QJsonObject rootObj;
	QJsonObject groupObj;
	dumpGroupToJson(groupObj,controls);
	rootObj["controls"]=groupObj;
	QJsonDocument doc;
	doc.setObject(rootObj);
	data=doc.toJson(QJsonDocument::Compact);
}

void ControlsParser::dumpToXml(QByteArray &data,const ControlsGroup &controls, bool shortStrings)
{
	QDomDocument doc;
	QDomElement rootElem=doc.createElement(shortStrings?"cls":"controls");
	doc.appendChild(rootElem);
	dumpToXml(rootElem,controls,shortStrings);
	data=doc.toByteArray();
}

void ControlsParser::dumpToXml(QDomElement &rootElem,const ControlsGroup &controls,bool shortStrings)
{
	QDomDocument doc=rootElem.ownerDocument();
	QDomElement groupElem=doc.createElement(shortStrings?"g":"group");
	rootElem.appendChild(groupElem);
	dumpGroupToXml(doc,groupElem,controls,shortStrings);
}

QList<ControlsCommand> ControlsGroup::extractCommandsList()const
{
	QList<ControlsCommand> retVal;
	extractCommandsList(retVal);
	return retVal;
}

QMap<QByteArray,ControlsCommand> ControlsGroup::extractCommandsMap() const
{
	QMap<QByteArray,ControlsCommand> retVal;
	extractCommandsMap(retVal);
	return retVal;
}

ControlsCommandParam::ControlsCommandParam()
{
	type=HIDDEN;
	layout=Qt::Vertical;
}

ControlsCommandParam::ControlsCommandParam(
	const QByteArray &tl,ControlsCommandParam::Type t,
	Qt::Orientation l,const QMap<QByteArray,QByteArray> &attrs)
{
	title=tl;
	type=t;
	layout=l;
	attributes=attrs;
}

bool ControlsCommandParam::operator==(const ControlsCommandParam &t)const
{
	return title==t.title&&type==t.type&&attributes==t.attributes&&layout==t.layout;
}

QByteArray ControlsCommandParam::typeToString(Type t)
{
	if(t==CHECKBOX)
		return "checkbox";
	else if(t==TEXT_EDIT)
		return "text_edit";
	else if(t==SELECT)
		return "select";
	else if(t==SLIDER)
		return "slider";
	else if(t==DIAL)
		return "dial";
	else if(t==RADIO)
		return "radio";
	else if(t==HIDDEN)
		return "hidden";
	else return QByteArray();
}

ControlsCommandParam::Type ControlsCommandParam::typeFromString(const QByteArray &s)
{
	if(s=="checkbox")
		return CHECKBOX;
	else if(s=="text_edit")
		return TEXT_EDIT;
	else if(s=="select")
		return SELECT;
	else if(s=="slider")
		return SLIDER;
	else if(s=="dial")
		return DIAL;
	else if(s=="radio")
		return RADIO;
	else if(s=="hidden")
		return HIDDEN;
	else return BAD_TYPE;
}

bool ControlsCommand::operator==(const ControlsCommand &t)const
{
	return title==t.title&&commandToExec==t.commandToExec&&layout==t.layout&&forceBtn==t.forceBtn&&
		params==t.params&&buttonText==t.buttonText;
}

ControlsGroup::Element::Element(ControlsCommand *c)
       :value(c)
{
	type=COMMAND;
}

ControlsGroup::Element::Element(ControlsGroup *g)
       :value(g)
{
	type=GROUP;
}

ControlsGroup::Element::Element(const ControlsCommand &c)
       :value(new ControlsCommand(c))
{
	type=COMMAND;
}

ControlsGroup::Element::Element(const ControlsGroup &g)
       :value(new ControlsGroup(g))
{
	type=GROUP;
}

ControlsGroup::Element::Element(const ControlsGroup::Element &t)
{
	*this=t;
}

ControlsGroup::Element& ControlsGroup::Element::operator=(const ControlsGroup::Element &t)
{
	type=t.type;
	if(type==GROUP)
	{
		ControlsGroup *g=new ControlsGroup(*t.group());
		value.reset(g);
	}
	else
	{
		ControlsCommand *c=new ControlsCommand(*t.command());
		value.reset(c);
	}
	return *this;
}

bool ControlsGroup::Element::operator==(const ControlsGroup::Element &t)const
{
	if(type!=t.type)return false;
	if(type==GROUP)
		return (*value->group())==(*t.value->group());
	else return (*value->command())==(*t.value->command());
}

bool ControlsGroup::Element::isGroup()const
{
	return type==GROUP;
}

bool ControlsGroup::Element::isCommand()const
{
	return type==COMMAND;
}

ControlsGroup* ControlsGroup::Element::group()
{
	if(type==GROUP)
		return value->group();
	return 0;
}

ControlsCommand* ControlsGroup::Element::command()
{
	if(type==COMMAND)
		return value->command();
	return 0;
}

const ControlsGroup* ControlsGroup::Element::group()const
{
	if(type==GROUP)
		return value->group();
	return 0;
}

const ControlsCommand* ControlsGroup::Element::command()const
{
	if(type==COMMAND)
		return value->command();
	return 0;
}
