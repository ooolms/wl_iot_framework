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

#include "ARpcPacketSensorValue.h"

ARpcPacketSensorValue::ARpcPacketSensorValue(quint32 dims)
{
	valueType=ARpcSensor::PACKET;
	dimensions=dims;
	if(dimensions==0)dimensions=1;
	valCount=0;
}

ARpcPacketSensorValue::ARpcPacketSensorValue(quint32 dims,bool localTimeStamp)
{
	if(localTimeStamp)valueType=ARpcSensor::PACKET_LT;
	else valueType=ARpcSensor::PACKET_GT;
	dimensions=dims;
	if(dimensions==0)dimensions=1;
	valCount=0;
}

ARpcSensor::Type ARpcPacketSensorValue::type()const
{
	return valueType;
}

bool ARpcPacketSensorValue::parse(const QByteArrayList &args)
{
	if(args.isEmpty())return false;
	if(valueType==ARpcSensor::PACKET&&args.count()!=1)return false;
	else if((valueType==ARpcSensor::PACKET_LT||valueType==ARpcSensor::PACKET_GT)&&args.count()!=2)return false;
	QByteArray packedValues;
	if(valueType!=ARpcSensor::PACKET)
	{
		bool ok=false;
		timestamp=args[0].toLongLong(&ok);
		if(!ok)return false;
		packedValues=QByteArray::fromBase64(args[1]);
	}
	else packedValues=QByteArray::fromBase64(args[0]);
	if(packedValues.size()%sizeof(float)!=0)return false;
	int numbersCount=packedValues.size()/sizeof(float);
	if(numbersCount%dimensions!=0)return false;
	valCount=numbersCount/dimensions;
	valuesList.resize(numbersCount);
	const float *fPackedValues=(const float*)packedValues.constData();
	for(int i=0;i<numbersCount;++i)
		valuesList[i]=fPackedValues[i];
	return true;
}

bool ARpcPacketSensorValue::parseF(const QByteArrayList &args)
{
	if(args.isEmpty())return false;
	if(valueType==ARpcSensor::PACKET&&args.count()!=1)return false;
	else if((valueType==ARpcSensor::PACKET_LT||valueType==ARpcSensor::PACKET_GT)&&args.count()!=2)return false;
	QByteArray packedValues;
	if(valueType!=ARpcSensor::PACKET)
	{
		if(args[0].size()!=8)
			return false;
		timestamp=*((qint64*)args[0].constData());
		packedValues=args[1];
	}
	else packedValues=args[0];
	if(packedValues.size()%sizeof(float)!=0)return false;
	int numbersCount=packedValues.size()/sizeof(float);
	if(numbersCount%dimensions!=0)return false;
	valCount=numbersCount/dimensions;
	valuesList.resize(numbersCount);
	const float *fPackedValues=(const float*)packedValues.constData();
	for(int i=0;i<numbersCount;++i)
		valuesList[i]=fPackedValues[i];
	return true;
}

bool ARpcPacketSensorValue::parseD(const QByteArrayList &args)
{
	if(args.isEmpty())return false;
	if(valueType==ARpcSensor::PACKET&&args.count()!=1)return false;
	else if((valueType==ARpcSensor::PACKET_LT||valueType==ARpcSensor::PACKET_GT)&&args.count()!=2)return false;
	QByteArray packedValues;
	if(valueType!=ARpcSensor::PACKET)
	{
		if(args[0].size()!=8)
			return false;
		timestamp=*((qint64*)args[0].constData());
		packedValues=args[1];
	}
	else packedValues=args[0];
	if(packedValues.size()%sizeof(double)!=0)return false;
	int numbersCount=packedValues.size()/sizeof(double);
	if(numbersCount%dimensions!=0)return false;
	valCount=numbersCount/dimensions;
	valuesList.resize(numbersCount);
	const double *fPackedValues=(const double*)packedValues.constData();
	for(int i=0;i<numbersCount;++i)
		valuesList[i]=fPackedValues[i];
	return true;
}

QByteArrayList ARpcPacketSensorValue::dump() const
{
	QByteArrayList retVal;
	if(valueType!=ARpcSensor::PACKET)
		retVal.append(QByteArray::number(timestamp));
	QByteArray packedValues=QByteArray((const char*)valuesList.data(),valuesList.count()*sizeof(ValueType)).toBase64();
	retVal.append(packedValues);
	return retVal;
}

ARpcISensorValue* ARpcPacketSensorValue::mkCopy()const
{
	ARpcPacketSensorValue *v=new ARpcPacketSensorValue(dimensions);
	v->valueType=valueType;
	v->valCount=valCount;
	v->valuesList=valuesList;
	return v;
}

const QVector<double>& ARpcPacketSensorValue::values()const
{
	return valuesList;
}

float ARpcPacketSensorValue::at(quint32 valIndex,quint32 dimension)const
{
	return valuesList[valIndex*dimensions+dimension];
}

void ARpcPacketSensorValue::fromData(const ValueType *vals,quint32 dims,quint32 count)
{
	dimensions=dims;
	valCount=count;
	valuesList.resize(dimensions*valCount);
	memcpy(valuesList.data(),vals,valCount*dimensions*sizeof(ValueType));
}

void ARpcPacketSensorValue::makeZeroFilledPacket(quint32 count)
{
	valCount=count;
	valuesList.resize(dimensions*count);
	valuesList.fill(0.0);
}

quint32 ARpcPacketSensorValue::dims()const
{
	return dimensions;
}

quint32 ARpcPacketSensorValue::valuesCount()const
{
	return valCount;
}

void ARpcPacketSensorValue::fromData(const QVector<ARpcPacketSensorValue::ValueType> &vals,quint32 dims)
{
	if(vals.size()%dims!=0)return;
	dimensions=dims;
	valCount=vals.size()/dims;
	valuesList=vals;
}
