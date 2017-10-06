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

#include "ARpcSingleSensorValue.h"

ARpcSingleSensorValue::ARpcSingleSensorValue(quint32 dims)
{
	valueType=ARpcSensor::SINGLE;
	dimensions=dims;
	if(dimensions==0)
		dimensions=1;
	valuesList.resize(dims);
}

ARpcSingleSensorValue::ARpcSingleSensorValue(quint32 dims,bool localTimeStamp)
{
	if(localTimeStamp)
		valueType=ARpcSensor::SINGLE_LT;
	else
		valueType=ARpcSensor::SINGLE_GT;
	dimensions=dims;
	if(dimensions==0)
		dimensions=1;
	valuesList.resize(dims);
	timestamp=0;
}

ARpcSensor::Type ARpcSingleSensorValue::type() const
{
	return valueType;
}

bool ARpcSingleSensorValue::parse(const QStringList &args)
{
	if(args.isEmpty())
		return false;
	if(valueType==ARpcSensor::SINGLE&&(quint32)args.count()!=dimensions)
		return false;
	else if((valueType==ARpcSensor::SINGLE_LT||valueType==ARpcSensor::SINGLE_GT)&&(quint32)args.count()!=(dimensions+1))
		return false;
	int valuesOffset=0;
	if(valueType!=ARpcSensor::SINGLE)
	{
		valuesOffset=1;
		bool ok=false;
		timestamp=args[0].toUtf8().toULongLong(&ok);
		if(!ok)
			return false;
	}
	valuesList.resize(dimensions);
	for(quint32 i=0;i<dimensions;++i)
	{
		bool ok=false;
		valuesList[i]=args[i+valuesOffset].toUtf8().toDouble(&ok);
		if(!ok)
			return false;
	}
	return true;
}

QStringList ARpcSingleSensorValue::dump() const
{
	QStringList retVal;
	if(valueType!=ARpcSensor::SINGLE)
		retVal.append(QByteArray::number(timestamp));
	for(int i=0;i<valuesList.count();++i)
		retVal.append(QByteArray::number(valuesList[i]));
	return retVal;
}

ARpcISensorValue* ARpcSingleSensorValue::mkCopy()
{
	ARpcSingleSensorValue *v=new ARpcSingleSensorValue(dimensions);
	v->valueType=valueType;
	v->valuesList=valuesList;
	return v;
}

const QVector<ARpcSingleSensorValue::ValueType>& ARpcSingleSensorValue::values() const
{
	return valuesList;
}

quint32 ARpcSingleSensorValue::dims() const
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
