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

#include "DBDriverHelpers.h"
#include <QDateTime>

using namespace WLIOT;

DBDriverHelpers::DBDriverHelpers()
{
	timeRule=ISensorStorage::DONT_TOUCH;
}

DBDriverHelpers::DBDriverHelpers(ISensorStorage::TimestampRule rule)
{
	timeRule=rule;
}

QByteArray DBDriverHelpers::packSensorValue(const SensorValue *val,int &hasTime,qint64 &timestamp)
{
	hasTime=0;
	timestamp=0;
	getTimestampForVal(val,hasTime,timestamp);
	QByteArray data;
	if(val->type().numType==SensorDef::TEXT)
	{
		if(hasTime)data.append((const char*)&timestamp,sizeof(qint64));
		const SensorValueText &textVal=(const SensorValueText&)*val;
		for(quint32 i=0;i<textVal.samplesCount();++i)
		{
			for(quint32 j=0;j<textVal.type().dim;++j)
			{
				QByteArray s=textVal.get(j,i);
				data.append(s.constData(),s.size()+1);
			}
		}
	}
	else
	{
		if(hasTime)data.append((const char*)&timestamp,sizeof(qint64));
		data.append(val->dumpToBinaryNoTime());
	}
	return data;
}

QByteArray DBDriverHelpers::packSensorValue(const SensorValue *val)
{
	int ht;
	qint64 ts;
	return packSensorValue(val,ht,ts);
}

SensorValue *DBDriverHelpers::unpackSensorValue(SensorDef::Type type,const QByteArray &data)
{
	if(type.numType==SensorDef::TEXT)
	{
		QByteArrayList msgArgs;
		bool hasTime=(type.tsType!=SensorDef::NO_TIME);
		if(hasTime)
		{
			if((quint32)data.size()<sizeof(qint64))return 0;
			qint64 t=*((const qint64*)data.constData());
			msgArgs.append(QByteArray::number(t));
		}
		QByteArray s;
		for(quint32 i=(hasTime?sizeof(qint64):0);i<(quint32)data.size();++i)
		{
			if(data[i])
				s.append(data[i]);
			else
			{
				msgArgs.append(s);
				s.clear();
			}
		}
		SensorValue *retVal=SensorValue::createSensorValue(type);
		if(retVal->parseMsgArgs(msgArgs))return retVal;
		delete retVal;
		return 0;
	}
	else
	{
		SensorValue *retVal=SensorValue::createSensorValue(type);
		if(retVal->parseBinary(data))return retVal;
		delete retVal;
		return 0;
	}
}

//void DBDriverHelpers::detectIfHasTime(ARpcSensorDef::Type type,int &hasTime)
//{
//	if(timeRule==ISensorStorage::ADD_GT)
//		hasTime=1;
//	else if(timeRule==ISensorStorage::DROP_TIME)
//		hasTime=0;
//	else hasTime=(type.tsType!=SensorDef::NO_TIME);
//}

void DBDriverHelpers::getTimestampForVal(const SensorValue *val,int &hasTime,qint64 &timestamp)
{
	hasTime=0;
	timestamp=0;
	if(timeRule==ISensorStorage::ADD_GT)
	{
		hasTime=1;
		if(val->type().tsType==SensorDef::GLOBAL_TIME)
			timestamp=val->time();
		else timestamp=QDateTime::currentMSecsSinceEpoch();
	}
	else if(timeRule==ISensorStorage::DONT_TOUCH)
	{
		hasTime=(val->type().tsType!=SensorDef::NO_TIME);
		if(hasTime)timestamp=val->time();
	}
}

QVector<quint32> DBDriverHelpers::sizesForFixedBlocksDb(SensorDef::Type type)
{
	QVector<quint32> retVal;
	if(type.packType==SensorDef::PACKET||type.numType==SensorDef::TEXT)
		return retVal;
	if(type.tsType!=SensorDef::NO_TIME)
		retVal.append(sizeof(qint64));
	for(quint32 i=0;i<type.dim;++i)
		retVal.append(type.valueSizeInBytes());
	return retVal;
}
