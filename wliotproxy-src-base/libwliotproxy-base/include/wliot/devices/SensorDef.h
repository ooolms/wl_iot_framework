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

#ifndef SENSORDEF_H
#define SENSORDEF_H

#include <QVariantMap>

class QDomElement;

namespace WLIOT
{
	/**
	 * @brief Описание датчика
	 * @details Значение датчика - один или несколько многомерных отсчетов ,состоящих из чисел определенного типа
	 * + возможна одна общая временная метка.
	 * Тип числа описывает перечисление NumType, наличие и тип временной метки - перечисление TimestampType.
	 */
	class SensorDef
	{
	public:
		/**
		 * @brief Тип значений, буква - F (floating point), S (signed integer) или U (unsigned integer),
		 * далее размерность в битах. Отдельно - текстовые данные (TEXT)
		 */
		enum NumType : unsigned char
		{
			BAD_TYPE,
			F32,//float
			F64,//double
			S8,//signed char
			U8,//unsigned char
			S16,//signed short
			U16,//unsigned short
			S32,//signed long
			U32,//unsigned long
			S64,//signed long long
			U64,//unsigned long long
			TEXT//char* UTF-8
		};

		/**
		 * @brief Отсчеты передаются всегда по одному (SINGLE) или их может быть переменное количество (PACKET)
		 */
		enum ValPackType : unsigned char
		{
			SINGLE,
			PACKET
		};

		/**
		 * @brief Описание временной метки передающихся данных
		 */
		enum TimestampType : unsigned char
		{
			NO_TIME,///< Нет временной метки
			LOCAL_TIME,///< Локальная временная метка (для устройств без часов глобального времени)
			GLOBAL_TIME///< Глобальная временная метка (unix time в миллисекундах)
		};

		/**
		 * @brief Описывает тип датчика
		 */
		class Type
		{
		public:
			Type()=default;
			Type(NumType nt,ValPackType pt,TimestampType tt,quint32 d);

		public:
			bool operator==(const Type &t)const;
			bool operator!=(const Type &t)const;
			bool isValid()const;
			bool isNumeric()const;///< все кроме текста
			bool isInteger()const;
			bool isSignedInteger()const;
			QByteArray toString()const;
			bool fromString(const QByteArray &str);
			/**
			 * @brief Возвращает true, когда размер значения в битах неизменен
			 * (то есть для единичных нетекстовых данных)
			 * @return
			 */
			bool hasFixedSize()const;
			/**
			 * @brief Размер одного числа из значения датчика в бинарном виде в байтах
			 * @details Если отсчеты передаются по одному, можно вычислить полный размер
			 * передаваемого в бинарном виде пакета с данными датчика. Для этого надо размер одного числа
			 * умножить на размерность и прибавить 8 байт, если есть временная метка.
			 * @return
			 */
			quint32 valueSizeInBytes()const;

		public:
			NumType numType=BAD_TYPE;///< Тип чисел в отсчетах значения датчика
			ValPackType packType=SINGLE;///< Количесиво отсчетов в значении (всегда один или переменное число)
			TimestampType tsType=NO_TIME;///< Тип временной метки
			quint32 dim=1;///< Размерность отсчета
		};

	public:
		SensorDef()=default;
		SensorDef(Type t,const QByteArray &n,const QByteArray &tl=QByteArray(),const QByteArray &u=QByteArray(),
			const QMap<QByteArray,QByteArray> &attrs=QMap<QByteArray,QByteArray>());
		QByteArray nameOrTitle()const;
		static int findByName(const QList<SensorDef> &sensors,const QByteArray &name);
		bool operator==(const SensorDef &t)const;

	public:
		QByteArray name;///< название датчика (рекомендуется на латинице без пробелов)
		QByteArray title;///< отображаемое пользователю название, может быть не задано, тогда выводится name
		QByteArray unit;///< единицы измерения
		Type type;///< тип датчика
		QMap<QByteArray,QByteArray> attributes;///< дополнительные атрибуты
	};

	/**
	 * @brief Преобразование списка датчиков в/из xml и json формата
	 */
	class SensorsParser
	{
	public:
		static bool parseJsonDescription(const QByteArray &data,QList<SensorDef> &sensors);
		static bool parseXmlDescription(const QByteArray &data,QList<SensorDef> &sensors);
		static bool parseXmlDescription(const QDomElement &rootElem, QList<SensorDef> &sensors,bool shortStrings=false);
		static void dumpToJson(QByteArray &data,const QList<SensorDef> &sensors);
		static void dumpToXml(QByteArray &data,const QList<SensorDef> &sensors,bool shortStrings=false);
		static void dumpToXml(QDomElement &rootElem,const QList<SensorDef> &sensors,bool shortStrings=false);
	};
}

Q_DECLARE_METATYPE(WLIOT::SensorDef::Type)

#endif // SENSORDEF_H
