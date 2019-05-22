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

#ifndef CONTROLSDEFINITION_H
#define CONTROLSDEFINITION_H

#include <QMap>
#include <QString>
#include <QSharedPointer>

class QJsonObject;
class QDomDocument;
class QDomElement;

class ControlsElement
{
public:
	virtual ~ControlsElement(){}
};

class ControlParam
	:public ControlsElement
{
public:
	enum Type
	{
		BAD_TYPE,
		CHECKBOX,
		TEXT_EDIT,
		SELECT,
		SLIDER,
		DIAL,
		RADIO,
		HIDDEN
	};

public:
	bool operator==(const ControlParam &t)const;
	static QByteArray typeToString(Type t);
	static Type typeFromString(const QByteArray &s);

public:
	QByteArray title;
	Type type=CHECKBOX;
	Qt::Orientation layout=Qt::Vertical;
	QMap<QByteArray,QByteArray> attributes;
};

class CommandControl
	:public ControlsElement
{
public:
	bool operator==(const CommandControl &t)const;

public:
	QByteArray title;
	QByteArray buttonText;
	QByteArray command;
	QList<ControlParam> params;
	Qt::Orientation layout=Qt::Vertical;
	bool forceBtn=false;
};

class ControlsGroup
	:public ControlsElement
{
public:
	class Element
	{
	public:
		enum Type{GROUP,CONTROL};

	public:
		explicit Element(const CommandControl &c);
		explicit Element(const ControlsGroup &g);
		Element(const Element &t);
		Element& operator=(const Element &t);
		bool operator==(const Element &t)const;
		bool isGroup()const;
		bool isControl()const;
		ControlsGroup* group();
		CommandControl* control();
		const ControlsGroup* group()const;
		const CommandControl* control()const;

	private:
		explicit Element(CommandControl *c);
		explicit Element(ControlsGroup *g);

	private:
		Type type;
		QScopedPointer<ControlsElement> value;
		friend class ControlsGroup;
	};

public:
	bool operator==(const ControlsGroup &t)const;
	static bool parseJsonDescription(const QByteArray &data,ControlsGroup &controls,bool ignoreSomeErrors=false);
	static bool parseXmlDescription(const QByteArray &data,ControlsGroup &controls,bool ignoreSomeErrors=false);
	static void dumpToJson(QByteArray &data,const ControlsGroup &controls);
	static void dumpToXml(QByteArray &data,const ControlsGroup &controls,bool shortTags=false);
	QList<CommandControl> extractCommandsList()const;

private:
	static bool parseJsonCommand(const QJsonObject &controlObject,CommandControl &control,
		bool shortStrings,bool ignoreSomeErrors);
	static bool parseJsonGroup(const QJsonObject &groupObject,ControlsGroup &group,
		bool shortStrings,bool ignoreSomeErrors);
	static bool parseXmlCommand(QDomElement &commandElem,CommandControl &command,
		bool shortStrings,bool ignoreSomeErrors);
	static bool parseXmlGroup(QDomElement &groupElem,ControlsGroup &group,
		bool shortStrings,bool ignoreSomeErrors);
	static void dumpControlToJson(QJsonObject &controlObj,const CommandControl &c);
	static void dumpGroupToJson(QJsonObject &groupObj,const ControlsGroup &g);
	static void dumpControlToXml(QDomDocument &doc,QDomElement &controlElem,
		const CommandControl &c,bool shortTags);
	static void dumpGroupToXml(QDomDocument &doc,QDomElement &groupElem,const ControlsGroup &grp,bool shortTags);
	void extractCommandsList(QList<CommandControl> &list)const;

public:
	QByteArray title;
	Qt::Orientation layout=Qt::Vertical;
	QList<Element> elements;
};

#endif // CONTROLSDEFINITION_H
