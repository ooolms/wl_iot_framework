#include "ARpcSingleSensorValue.h"

ARpcSingleSensorValue::ARpcSingleSensorValue(quint32 dims)
{
	valueType=ARpcSensor::SINGLE;
	dimensions=dims;
	if(dimensions==0)dimensions=1;
	valuesList.resize(dims);
	timestamp=0;
}

ARpcSingleSensorValue::ARpcSingleSensorValue(quint32 dims,bool localTimeStamp)
{
	if(localTimeStamp)valueType=ARpcSensor::SINGLE_LT;
	else valueType=ARpcSensor::SINGLE_GT;
	dimensions=dims;
	if(dimensions==0)dimensions=1;
	valuesList.resize(dims);
	timestamp=0;
}

ARpcSensor::Type ARpcSingleSensorValue::type()const
{
	return valueType;
}

bool ARpcSingleSensorValue::parse(ARpcMessage m)
{
	if(m.args.isEmpty())return false;
	m.args.removeFirst();
	if(valueType==ARpcSensor::SINGLE&&(quint32)m.args.count()!=dimensions)return false;
	else if((valueType==ARpcSensor::SINGLE_LT||valueType==ARpcSensor::SINGLE_GT)&&
		(quint32)m.args.count()!=(dimensions+1))return false;
	int valuesOffset=0;
	if(valueType!=ARpcSensor::SINGLE)
	{
		valuesOffset=1;
		bool ok=false;
		timestamp=m.args[0].toUtf8().toLongLong(&ok);
		if(!ok)return false;
	}
	valuesList.resize(dimensions);
	for(quint32 i=0;i<dimensions;++i)
	{
		bool ok=false;
		valuesList[i]=m.args[i+valuesOffset].toDouble(&ok);
		if(!ok)return false;
	}
	return true;
}

const QVector<ARpcSingleSensorValue::ValueType>& ARpcSingleSensorValue::values()const
{
	return valuesList;
}

qint64 ARpcSingleSensorValue::time()const
{
	return timestamp;
}

void ARpcSingleSensorValue::setTime(qint64 t)
{
	timestamp=t;
}

quint32 ARpcSingleSensorValue::dims()const
{
	return dimensions;
}

void ARpcSingleSensorValue::fromData(const QVector<ValueType> &vals)
{
	dimensions=vals.count();
	valuesList=vals;
}

void ARpcSingleSensorValue::fromData(const ValueType *vals,quint32 dims)
{
	dimensions=dims;
	valuesList.resize(dims);
	memcpy(valuesList.data(),vals,dims*sizeof(ARpcSingleSensorValue::ValueType));
}
