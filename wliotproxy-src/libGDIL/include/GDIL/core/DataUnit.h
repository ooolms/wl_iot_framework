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
		BOOL=0x4,
		ANY=0x7
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
	explicit DataUnit(Type t,quint32 dim,NumericType numType=F64);
	explicit DataUnit(const SensorValue *v);
	explicit DataUnit(const QVector<SensorValue*> &vList);
	explicit DataUnit(double v);
	explicit DataUnit(qint64 v);
	explicit DataUnit(bool v);
	DataUnit(const DataUnit &t);
	DataUnit& operator=(const DataUnit &t);
	~DataUnit();
	bool isValid()const;
	Type type()const;
	NumericType numType()const;
	const SensorValue *value()const;
	DataUnit mkCopy()const;
	quint32 dim()const;
	bool valueFromArgs(const QByteArrayList &args);

	static bool canCreateFromValue(SensorDef::Type t);
	static bool canCreateFromArrayOfValues(SensorDef::Type t);
	static Type typeForSensorValue(SensorDef::Type t,bool singleValue);

private:
	void constructByType(Type t,quint32 dim,NumericType numType=F64);
	void calcNumType();
	void derefValue();

private:
	int *mValueRefCount;
	Type mType;
	NumericType mNumType;
	SensorValue *mValue;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DataUnit::Types)

#endif // DATAUNIT_H
