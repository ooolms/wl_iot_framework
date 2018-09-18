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

#include "ARpcDBDriverHelpers.h"
#include <QDateTime>

ARpcDBDriverHelpers::ARpcDBDriverHelpers()
{
	timeRule=ARpcISensorStorage::DONT_TOUCH;
}

ARpcDBDriverHelpers::ARpcDBDriverHelpers(ARpcISensorStorage::TimestampRule rule)
{
	timeRule=rule;
}

QByteArray ARpcDBDriverHelpers::packSensorValue(const ARpcSensorValue *val,int &hasTime,qint64 &timestamp)
{
	hasTime=0;
	timestamp=0;
	getTimestampForVal(val,hasTime,timestamp);
	QByteArray data;
	if(val->type().numType==ARpcSensorDef::TEXT)
	{
		if(hasTime)data.append((const char*)&timestamp,sizeof(qint64));
		const ARpcSensorValueText &textVal=(const ARpcSensorValueText&)*val;
		for(quint32 i=0;i<textVal.packetsCount();++i)
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

QByteArray ARpcDBDriverHelpers::packSensorValue(const ARpcSensorValue *val)
{
	int ht;
	qint64 ts;
	return packSensorValue(val,ht,ts);
}

ARpcSensorValue *ARpcDBDriverHelpers::unpackSensorValue(ARpcSensorDef::Type type,const QByteArray &data)
{
	if(type.numType==ARpcSensorDef::TEXT)
	{
		QByteArrayList msgArgs;
		bool hasTime=(type.tsType!=ARpcSensorDef::NO_TIME);
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
		ARpcSensorValue *retVal=ARpcSensorValue::createSensorValue(type);
		if(retVal->parseMsgArgs(msgArgs))return retVal;
		delete retVal;
		return 0;
	}
	else
	{
		ARpcSensorValue *retVal=ARpcSensorValue::createSensorValue(type);
		if(retVal->parseBinary(data))return retVal;
		delete retVal;
		return 0;
	}
}

/*void ARpcDBDriverHelpers::detectIfHasTime(ARpcSensorDef::Type type,int &hasTime)
{
	if(timeRule==ARpcISensorStorage::ADD_GT)
		hasTime=1;
	else if(timeRule==ARpcISensorStorage::DROP_TIME)
		hasTime=0;
	else hasTime=(type.tsType!=ARpcSensorDef::NO_TIME);
}*/

void ARpcDBDriverHelpers::getTimestampForVal(const ARpcSensorValue *val,int &hasTime,qint64 &timestamp)
{
	hasTime=0;
	timestamp=0;
	if(timeRule==ARpcISensorStorage::ADD_GT)
	{
		hasTime=1;
		if(val->type().tsType==ARpcSensorDef::GLOBAL_TIME)
			timestamp=val->time();
		else timestamp=QDateTime::currentMSecsSinceEpoch();
	}
	else if(timeRule==ARpcISensorStorage::DONT_TOUCH)
	{
		hasTime=(val->type().tsType!=ARpcSensorDef::NO_TIME);
		if(hasTime)timestamp=val->time();
	}
}

QVector<quint32> ARpcDBDriverHelpers::sizesForFixedBlocksDb(ARpcSensorDef::Type type)
{
	QVector<quint32> retVal;
	if(type.packType==ARpcSensorDef::PACKET||type.numType==ARpcSensorDef::TEXT)
		return retVal;
	if(type.tsType!=ARpcSensorDef::NO_TIME)
		retVal.append(sizeof(qint64));
	for(quint32 i=0;i<type.dim;++i)
		retVal.append(type.valueSizeInBytes());
	return retVal;
}
