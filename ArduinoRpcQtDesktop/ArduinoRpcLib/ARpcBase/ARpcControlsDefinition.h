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

#ifndef ARPCCONTROLSDEFINITION_H
#define ARPCCONTROLSDEFINITION_H

#include <QMap>
#include <QString>
#include <QSharedPointer>

class ARpcControlsElement
{
public:
	virtual ~ARpcControlsElement(){}
};

class ARpcControlParam
	:public ARpcControlsElement
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
	bool operator==(const ARpcControlParam &t)const;
	static QByteArray typeToString(Type t);
	static Type typeFromString(const QByteArray &s);

public:
	QByteArray title;
	Type type=CHECKBOX;
	QMap<QByteArray,QByteArray> attributes;
};

class ARpcCommandControl
	:public ARpcControlsElement
{
public:
	bool operator==(const ARpcCommandControl &t)const;

public:
	QByteArray title;
	QByteArray command;
	QList<ARpcControlParam> params;
	Qt::Orientation layout=Qt::Vertical;
	bool forceBtn=false;
};

class ARpcControlsGroup
	:public ARpcControlsElement
{
public:
	class Element
	{
	public:
		enum Type{GROUP,CONTROL};

	public:
		explicit Element(ARpcCommandControl *c);
		explicit Element(ARpcControlsGroup *g);
		Element(const Element &t);
		Element& operator=(const Element &t);
		bool operator==(const Element &t)const;
		bool isGroup()const;
		bool isControl()const;
		ARpcControlsGroup* group();
		ARpcCommandControl* control();
		const ARpcControlsGroup* group()const;
		const ARpcCommandControl* control()const;

	private:
		Type type;
		QScopedPointer<ARpcControlsElement> value;
	};

public:
	bool operator==(const ARpcControlsGroup &t)const;
	static bool parseJsonDescription(const QByteArray &data,ARpcControlsGroup &controls);
	static bool parseXmlDescription(const QByteArray &data,ARpcControlsGroup &controls);
	static void dumpToJson(QByteArray &data,const ARpcControlsGroup &controls);
	static void dumpToXml(QByteArray &data,const ARpcControlsGroup &controls);

public:
	QByteArray title;
	Qt::Orientation layout=Qt::Vertical;
	QList<Element> elements;
};

#endif // ARPCCONTROLSDEFINITION_H
