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

#ifndef DATAUNIT_H
#define DATAUNIT_H

#include "wliot/devices/SensorValue.h"
#include <QDateTime>

namespace WLIOTVDIL
{
	class DataUnit
	{
	public:
		enum Type
		{
			INVALID=0x0,
			SINGLE=0x1,
			ARRAY=0x2,
			BOOL=0x4,
			DATETIME=0x8,
			ANY=SINGLE|ARRAY|BOOL|DATETIME
		};
		Q_DECLARE_FLAGS(Types,Type)
		//Числовой тип используется для возможности осуществлять операции над целыми числами, когда при этом можно
		//избежать потери точности (например, при преобразовании из float в double), например, точное сравнение
		//целых чисел вместо сравнения вещественных
		enum NumericType
		{
			F64,
			S64
		};

	public:
		DataUnit();
		explicit DataUnit(Type t,quint32 dim);
		explicit DataUnit(Type t,quint32 dim,const QByteArrayList &msgArgs);
		explicit DataUnit(const WLIOT::SensorValue *v);
		explicit DataUnit(const QVector<WLIOT::SensorValue*> &vList);
		explicit DataUnit(double v);
		explicit DataUnit(qint64 v);
		explicit DataUnit(bool v);
		explicit DataUnit(const QDateTime &d);
		explicit DataUnit(const QVector<double> &vals);
		explicit DataUnit(const QVector<qint64> &vals);
		DataUnit(const DataUnit &t);
		DataUnit& operator=(const DataUnit &t);
		~DataUnit();
		bool isValid()const;
		Type type()const;
		NumericType numType()const;
		const WLIOT::SensorValue *value()const;
		quint32 dim()const;
		bool parseMsgArgs(const QByteArrayList &args);
		DataUnit mkCopy();
		QByteArrayList toMsgArgs()const;
		void swap(DataUnit &t);
		bool boolValue()const;
		QDateTime dateTimeValue()const;

	public:
		static Type typeForSensorValue(WLIOT::SensorDef::Type t,bool singleValue);
		static DataUnit single1DimValueFromString(const QString &s);
		static QByteArray typeToStr(DataUnit::Type t);
		static DataUnit::Type typeFromStr(const QByteArray &str);

	private:
		static bool canCreateFromValue(WLIOT::SensorDef::Type t);
		static bool canCreateFromArrayOfValues(WLIOT::SensorDef::Type t);
		void constructByType(Type t,quint32 dim,NumericType numType=F64);
		void calcNumType();
		void derefValue();

	private:
		int *mValueRefCount;
		Type mType;
		NumericType mNumType;
		WLIOT::SensorValue *mValue;
	};

	Q_DECLARE_OPERATORS_FOR_FLAGS(DataUnit::Types)

	class TypeConstraints
	{
	public:
		TypeConstraints();
		explicit TypeConstraints(DataUnit::Types t,quint32 d);
		bool match(DataUnit::Type t,quint32 d)const;
		bool match(const DataUnit &u)const;

	public:
		DataUnit::Types types;
		quint32 dim;//0 - no limit, >=1 - fixed dim only
	};
}

#endif // DATAUNIT_H
