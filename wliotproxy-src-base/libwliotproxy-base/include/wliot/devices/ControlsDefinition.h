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

/**
  * @file В этом файле содержатся классы, описывающие интерфейс управления устройством
  * @see https://dev.alterozoom.com/doc/doku.php?id=iot:%D0%BF%D1%80%D0%BE%D1%82%D0%BE%D0%BA%D0%BE%D0%BB_%D0%B4%D0%BB%D1%8F_%D1%83%D1%81%D1%82%D1%80%D0%BE%D0%B9%D1%81%D1%82%D0%B2#%D1%84%D0%BE%D1%80%D0%BC%D0%B0%D1%82_%D0%BE%D0%BF%D0%B8%D1%81%D0%B0%D0%BD%D0%B8%D1%8F_%D0%B8%D0%BD%D1%82%D0%B5%D1%80%D1%84%D0%B5%D0%B9%D1%81%D0%B0_%D1%83%D0%BF%D1%80%D0%B0%D0%B2%D0%BB%D0%B5%D0%BD%D0%B8%D1%8F
  */

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

	/**
	 * @brief Базовый класс для всех элементов интерфейса управления.
	 */
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

	/**
	 * @brief Параметр команды
	 */
	class ControlsCommandParam
		:public ControlsElement
	{
	public:
		///Тип элемента управления
		enum Type
		{
			BAD_TYPE,///< невалидный тип
			CHECKBOX,///< чекбокс
			TEXT_EDIT,///< однострочное поле ввода
			SELECT,///< выпадающий список
			SLIDER,///< слайдер
			DIAL,///< "крутилка"
			RADIO,///< группа радиокнопок
			HIDDEN///< неотображаемый параметр с константным значением
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
		QByteArray title;///< заголовок
		Type type;///< тип элемента
		Qt::Orientation layout;///< горизонтальное/вертикальное размещение заголовка и самого элемента
		QMap<QByteArray,QByteArray> attributes;///< атрибуты (@see https://dev.alterozoom.com/doc/doku.php?id=iot:%D0%BF%D1%80%D0%BE%D1%82%D0%BE%D0%BA%D0%BE%D0%BB_%D0%B4%D0%BB%D1%8F_%D1%83%D1%81%D1%82%D1%80%D0%BE%D0%B9%D1%81%D1%82%D0%B2#%D0%B0%D1%82%D1%80%D0%B8%D0%B1%D1%83%D1%82%D1%8B_%D0%BF%D0%B0%D1%80%D0%B0%D0%BC%D0%B5%D1%82%D1%80%D0%BE%D0%B2_%D0%BA%D0%BE%D0%BC%D0%B0%D0%BD%D0%B4)
	};

	/**
	 * @brief Команда устройству, включает в себя список параметров
	 */
	class ControlsCommand
		:public ControlsElement
	{
	public:
		bool operator==(const ControlsCommand &t)const;
		virtual bool isCommand()const override{return true;}
		virtual ControlsCommand* command()override{return this;}
		virtual const ControlsCommand* command()const override{return this;}

	public:
		QByteArray title;///< заголовок команды
		QByteArray buttonText;///< текст для кнопки отправки (Send по-умолчанию)
		QByteArray commandToExec;///< команда, отправляемая устройству
		QList<ControlsCommandParam> params;///< параметры команды
		Qt::Orientation layout=Qt::Vertical;///< горизонтальное/вертикальное размещение параметров
		bool forceBtn=false;///< принудительно отображать кнопку отправки команды
	};

	/**
	 * @brief Группа элементов управления, в которую входят команды и другие группы
	 */
	class ControlsGroup
		:public ControlsElement
	{
	public:
		/**
		 * @brief Элемент группы, команда или подгруппа
		 */
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
		/**
		 * @brief Извлечь список всех команд (рекурсивно для подгрупп)
		 * @return спислок команд в контейнере QList
		 */
		QList<ControlsCommand> extractCommandsList()const;
		/**
		 * @brief Извлечь список всех команд (рекурсивно для подгрупп)
		 * @return спислок команд в контейнере QMap (ключ - команда для устройства)
		 */
		QMap<QByteArray,ControlsCommand> extractCommandsMap()const;

	private:
		void extractCommandsList(QList<ControlsCommand> &list)const;
		void extractCommandsMap(QMap<QByteArray,ControlsCommand> &map)const;

	public:
		QByteArray title;///< заголовок группы
		Qt::Orientation layout=Qt::Vertical;///< горизонтальное/вертикальное размещение элементов группы
		QList<Element> elements;///< список элементов
	};

	/**
	 * @brief Преобразование интерфейса управления в/из xml и json форматов
	 */
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
