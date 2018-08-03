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
/*
ARpcSingleSensorValue::ARpcSingleSensorValue(quint32 dims)
{
	valueType=ARpcSensorDef::SINGLE;
	dimensions=dims;
	if(dimensions==0)
		dimensions=1;
	valuesList.resize(dims);
}

ARpcSingleSensorValue::ARpcSingleSensorValue(quint32 dims,bool localTimeStamp)
{
	if(localTimeStamp)
		valueType=ARpcSensorDef::SINGLE_LT;
	else
		valueType=ARpcSensorDef::SINGLE_GT;
	dimensions=dims;
	if(dimensions==0)
		dimensions=1;
	valuesList.resize(dims);
	mTime=0;
}

ARpcSensorDef::Type ARpcSingleSensorValue::type() const
{
	return valueType;
}

bool ARpcSingleSensorValue::parseMsgArgs(const QByteArrayList &args)
{
	if(args.isEmpty())
		return false;
	if(valueType==ARpcSensorDef::SINGLE&&(quint32)args.count()!=dimensions)
		return false;
	else if((valueType==ARpcSensorDef::SINGLE_LT||valueType==ARpcSensorDef::SINGLE_GT)&&(quint32)args.count()!=(dimensions+1))
		return false;
	int valuesOffset=0;
	if(valueType!=ARpcSensorDef::SINGLE)
	{
		valuesOffset=1;
		bool ok=false;
		mTime=args[0].toULongLong(&ok);
		if(!ok)
			return false;
	}
	valuesList.resize(dimensions);
	for(quint32 i=0;i<dimensions;++i)
	{
		bool ok=false;
		valuesList[i]=args[i+valuesOffset].toDouble(&ok);
		if(!ok)
			return false;
	}
	return true;
}

bool ARpcSingleSensorValue::parseF(const QByteArrayList &args)
{
	if(args.isEmpty())
		return false;
	if(valueType==ARpcSensorDef::SINGLE&&(quint32)args.count()!=dimensions)
		return false;
	else if((valueType==ARpcSensorDef::SINGLE_LT||valueType==ARpcSensorDef::SINGLE_GT)&&(quint32)args.count()!=(dimensions+1))
		return false;
	int valuesOffset=0;
	if(valueType!=ARpcSensorDef::SINGLE)
	{
		valuesOffset=1;
		if(args[0].size()!=8)
			return false;
		mTime=*((qint64*)args[0].constData());
	}
	valuesList.resize(dimensions);
	for(quint32 i=0;i<dimensions;++i)
	{
		if(args[i+valuesOffset].size()!=4)return false;
		valuesList[i]=*((float*)args[i+valuesOffset].constData());
	}
	return true;
}

bool ARpcSingleSensorValue::parseD(const QByteArrayList &args)
{
	if(args.isEmpty())
		return false;
	if(valueType==ARpcSensorDef::SINGLE&&(quint32)args.count()!=dimensions)
		return false;
	else if((valueType==ARpcSensorDef::SINGLE_LT||valueType==ARpcSensorDef::SINGLE_GT)&&(quint32)args.count()!=(dimensions+1))
		return false;
	int valuesOffset=0;
	if(valueType!=ARpcSensorDef::SINGLE)
	{
		valuesOffset=1;
		if(args[0].size()!=8)
			return false;
		mTime=*((qint64*)args[0].constData());
	}
	valuesList.resize(dimensions);
	for(quint32 i=0;i<dimensions;++i)
	{
		if(args[i+valuesOffset].size()!=8)return false;
		valuesList[i]=*((double*)args[i+valuesOffset].constData());
	}
	return true;
}

QByteArrayList ARpcSingleSensorValue::dump()const
{
	QByteArrayList retVal;
	if(valueType!=ARpcSensorDef::SINGLE)
		retVal.append(QByteArray::number(mTime));
	for(int i=0;i<valuesList.count();++i)
		retVal.append(QByteArray::number(valuesList[i]));
	return retVal;
}

ARpcSensorValue* ARpcSingleSensorValue::mkCopy()const
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
*/
