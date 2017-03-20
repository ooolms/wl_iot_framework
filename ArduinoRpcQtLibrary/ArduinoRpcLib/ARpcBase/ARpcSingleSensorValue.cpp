#include "ARpcSingleSensorValue.h"

ARpcSingleSensorValue::ARpcSingleSensorValue(int dims)
{
	valueType=ARpcSensor::SINGLE;
	dimensions=dims;
	if(dimensions<=0)dimensions=1;
	timestamp=0;
}

ARpcSingleSensorValue::ARpcSingleSensorValue(int dims,bool localTimeStamp)
{
	if(localTimeStamp)valueType=ARpcSensor::SINGLE_LT;
	else valueType=ARpcSensor::SINGLE_GT;
	dimensions=dims;
	if(dimensions<=0)dimensions=1;
}

ARpcSensor::Type ARpcSingleSensorValue::type()const
{
	return valueType;
}

bool ARpcSingleSensorValue::parse(const ARpcMessage &m)
{
	if(valueType==ARpcSensor::SINGLE&&m.args.count()!=dimensions)return false;
	else if((valueType==ARpcSensor::SINGLE_LT||valueType==ARpcSensor::SINGLE_GT)&&
		m.args.count()!=(dimensions+1))return false;
	int valuesOffset=0;
	if(valueType!=ARpcSensor::SINGLE)
	{
		valuesOffset=1;
		bool ok=false;
		timestamp=m.args[0].toLongLong(&ok);
		if(!ok)return false;
	}
	valuesList.resize(dimensions);
	for(int i=0;i<dimensions;++i)
	{
		bool ok=false;
		valuesList[i]=m.args[i+valuesOffset].toDouble(&ok);
		if(!ok)return false;
	}
	return true;
}

const QVector<double> &ARpcSingleSensorValue::values()const
{
	return valuesList;
}
