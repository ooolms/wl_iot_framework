#ifndef DATAUNIT_H
#define DATAUNIT_H

#include "wliot/devices/SensorValue.h"

class DataUnit
{
public:
	enum Type
	{
		INVALID=0x0,
		SINGLE=0x1,
		ARRAY=0x2,
		BOOL=0x4
	};
	Q_DECLARE_FLAGS(Types,Type)
	//Числовой тип используется для возможности осуществлять операции над целыми числами, когда при этом можно
	//избежать потери точности (например, при преобразовании из float в double), например, точное сравнение
	//целых чисел вместо сравнения вещественных
	enum NumericType
	{
		F32,
		F64,
		S64,
		U64
	};

public:
	explicit DataUnit(Type t);
	explicit DataUnit(const SensorValue *v);
	explicit DataUnit(const QVector<SensorValue*> &vList);
	DataUnit(const DataUnit &t);
	DataUnit& operator=(const DataUnit &t);
	~DataUnit();
	bool isValid()const;
	Type type()const;
	NumericType numType()const;
	const SensorValue *value();
	DataUnit mkCopy()const;

	static bool canCreateFromValue(SensorDef::Type t);
	static bool canCreateFromArrayOfValues(SensorDef::Type t);

private:
	DataUnit();
	void constructByType(Type t);
	void calcNumType();
	void derefValue();

private:
	int *mValueRefCount;
	Type mType;
	NumericType mNumType;
	SensorValue *mValue;
};

#endif // DATAUNIT_H
