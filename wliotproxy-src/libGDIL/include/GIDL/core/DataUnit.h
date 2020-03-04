#ifndef DATAUNIT_H
#define DATAUNIT_H

#include "wliot/devices/SensorValue.h"

class DataUnit
{
public:
	enum class Type
	{
		SINGLE,
		ARRAY,
		BOOL,
		TEXT
	};
	//Числовой тип используется для возможности осуществлять операции над целыми числами, когда при этом можно
	//избежать потери точности (например, при преобразовании из float в double), например, точное сравнение
	//целых чисел вместо сравнения вещественных
	enum class NumericType
	{
		F32,
		F64,
		S64,
		U64
	};

public:
	explicit DataUnit(Type t);
	explicit DataUnit(SensorValue *v);
	explicit DataUnit(const QVector<SensorValue> &vList);
	Type type()const;

private:
	DataUnit(const DataUnit &t);
	DataUnit& operator=(const DataUnit &)=delete;

private:
	Type mType;
	SensorValue *mValue;
};

#endif // DATAUNIT_H
