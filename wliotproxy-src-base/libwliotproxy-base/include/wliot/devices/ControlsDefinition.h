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

namespace WLIOT
{
	class ControlsCommandParam;
	class ControlsCommand;
	class ControlsGroup;

	class ControlsElement
	{
	public:
		enum Type
		{
			BAD_TYPE,
			GROUP,
			COMMAND,
			COMMAND_PARAM
		};

	public:
		virtual ~ControlsElement(){}
		virtual bool isGroup()const{return false;}
		virtual bool isCommand()const{return false;}
		virtual bool isCommandParam()const{return false;}
		virtual ControlsGroup* group(){return 0;}
		virtual ControlsCommand* command(){return 0;}
		virtual ControlsCommandParam* commandParam(){return 0;}
		virtual const ControlsGroup* group()const{return 0;}
		virtual const ControlsCommand* command()const{return 0;}
		virtual const ControlsCommandParam* commandParam()const{return 0;}
	};

	class ControlsCommandParam
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
		ControlsCommandParam();
		explicit ControlsCommandParam(const QByteArray &tl,Type t,Qt::Orientation l=Qt::Vertical,
			const QMap<QByteArray,QByteArray> &attrs=QMap<QByteArray,QByteArray>());
		bool operator==(const ControlsCommandParam &t)const;
		static QByteArray typeToString(Type t);
		static Type typeFromString(const QByteArray &s);
		virtual bool isCommandParam()const override{return true;}
		virtual ControlsCommandParam* commandParam()override{return this;}
		virtual const ControlsCommandParam* commandParam()const override{return this;}

	public:
		QByteArray title;
		Type type;
		Qt::Orientation layout;
		QMap<QByteArray,QByteArray> attributes;
	};

	class ControlsCommand
		:public ControlsElement
	{
	public:
		bool operator==(const ControlsCommand &t)const;
		virtual bool isCommand()const override{return true;}
		virtual ControlsCommand* command()override{return this;}
		virtual const ControlsCommand* command()const override{return this;}

	public:
		QByteArray title;
		QByteArray buttonText;
		QByteArray commandToExec;
		QList<ControlsCommandParam> params;
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
			enum Type{GROUP,COMMAND};

		public:
			explicit Element(const ControlsCommand &c);
			explicit Element(const ControlsGroup &g);
			explicit Element(ControlsCommand *c);
			explicit Element(ControlsGroup *g);
			Element(const Element &t);
			Element& operator=(const Element &t);
			bool operator==(const Element &t)const;
			bool isGroup()const;
			bool isCommand()const;
			ControlsGroup* group();
			ControlsCommand* command();
			const ControlsGroup* group()const;
			const ControlsCommand* command()const;

		private:
			Type type;
			QScopedPointer<ControlsElement> value;
			friend class ControlsGroup;
		};

	public:
		bool operator==(const ControlsGroup &t)const;
		virtual bool isGroup()const override{return true;}
		virtual ControlsGroup* group()override{return this;}
		virtual const ControlsGroup* group()const override{return this;}
		QList<ControlsCommand> extractCommandsList()const;
		QMap<QByteArray,ControlsCommand> extractCommandsMap()const;

	private:
		void extractCommandsList(QList<ControlsCommand> &list)const;
		void extractCommandsMap(QMap<QByteArray,ControlsCommand> &map)const;

	public:
		QByteArray title;
		Qt::Orientation layout=Qt::Vertical;
		QList<Element> elements;
	};

	class ControlsParser
	{
	public:
		static bool parseJsonDescription(const QByteArray &data,ControlsGroup &controls,bool ignoreSomeErrors=false);
		static bool parseXmlDescription(const QByteArray &data,ControlsGroup &controls,bool ignoreSomeErrors=false);
		static bool parseXmlDescription(const QDomElement &rootElem,ControlsGroup &controls,
			bool ignoreSomeErrors=false,bool shortStrings=false);
		static void dumpToJson(QByteArray &data,const ControlsGroup &controls);
		static void dumpToXml(QByteArray &data,const ControlsGroup &controls,bool shortStrings=false);
		static void dumpToXml(QDomElement &rootElem,const ControlsGroup &controls,bool shortStrings=false);

	private:
		static bool parseJsonCommand(const QJsonObject &controlObject,ControlsCommand &control,
			bool shortStrings,bool ignoreSomeErrors);
		static bool parseJsonGroup(const QJsonObject &groupObject,ControlsGroup &group,
			bool shortStrings,bool ignoreSomeErrors);
		static bool parseXmlCommand(QDomElement &commandElem,ControlsCommand &command,
			bool shortStrings,bool ignoreSomeErrors);
		static bool parseXmlGroup(QDomElement &groupElem,ControlsGroup &group,
			bool shortStrings,bool ignoreSomeErrors);
		static void dumpControlToJson(QJsonObject &controlObj,const ControlsCommand &c);
		static void dumpGroupToJson(QJsonObject &groupObj,const ControlsGroup &g);
		static void dumpControlToXml(QDomDocument &doc,QDomElement &controlElem,
			const ControlsCommand &c,bool shortStrings);
		static void dumpGroupToXml(QDomDocument &doc,QDomElement &groupElem,const ControlsGroup &grp,bool shortStrings);
	};
}

#endif // CONTROLSDEFINITION_H
