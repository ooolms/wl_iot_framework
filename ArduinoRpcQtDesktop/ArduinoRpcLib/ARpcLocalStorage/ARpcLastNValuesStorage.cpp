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

#include "ARpcLastNValuesStorage.h"
#include "ARpcBase/ARpcPacketSensorValue.h"
#include "ARpcBase/ARpcTextSensorValue.h"
#include "ARpcBase/ARpcSingleSensorValue.h"
#include <QDateTime>

ARpcLastNValuesStorage::ARpcLastNValuesStorage(ARpcSensor::Type valType,QObject *parent)
	:ARpcISensorStorage(valType,parent)
{
	storedCount=0;
	opened=false;
	timestampRule=ADD_GT;
	effectiveValType=defaultEffectiveValuesType(timestampRule);
}

ARpcLastNValuesStorage::~ARpcLastNValuesStorage()
{
	close();
}

bool ARpcLastNValuesStorage::create(quint32 storedValuesCount,const ARpcISensorValue &fillerValue)
{
	if(opened)return false;
	if(storedValuesCount==0)storedValuesCount=1;
	effectiveValType=defaultEffectiveValuesType(timestampRule);
	QFile file(dbDir.absolutePath()+"/db.index");
	if(file.exists())return false;
	storedCount=storedValuesCount;
	QSettings settings(dbDir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	settings.setValue("stored_count",QString::number(storedCount));
	settings.sync();
	if(settings.status()!=QSettings::NoError)return false;
	if(!file.open(QIODevice::WriteOnly))return false;
	file.write("0");
	file.close();
	opened=true;
	values.clear();
	hlp=ARpcDBDriverHelpers(timestampRule);
	startIndex=0;
	for(quint32 i=0;i<storedCount;++i)
		if(!writeSensorValue(&fillerValue))return false;
	return true;
}

bool ARpcLastNValuesStorage::isOpened() const
{
	return opened;
}

ARpcISensorStorage::StoreMode ARpcLastNValuesStorage::getStoreMode()const
{
	return ARpcISensorStorage::LAST_N_VALUES;
}

bool ARpcLastNValuesStorage::writeSensorValue(const ARpcISensorValue *val)
{
	if(!opened)return false;
	int hasTime;
	qint64 ts;
	QByteArray data=hlp.packSensorValue(val,hasTime,ts);
	if(data.isEmpty())return false;
	QFile dataFile(dbDir.absolutePath()+"/"+QString::number((startIndex+storedCount-1)%storedCount)+".data");
	QFile indexFile(dbDir.absolutePath()+"/db.index");
	if(!dataFile.open(QIODevice::WriteOnly)||!indexFile.open(QIODevice::WriteOnly))return false;
	indexFile.write(QByteArray::number((startIndex+storedCount-1)%storedCount));
	indexFile.close();
	startIndex=(startIndex+storedCount-1)%storedCount;
	if(dataFile.write(data)!=data.size())
	{
		dataFile.close();
		dataFile.remove();
	}
	else dataFile.close();
	if((quint32)values.count()>storedCount)
	{
		delete values.last();
		values.removeLast();
	}
	ARpcISensorValue *newVal=valueFromDisk(0);
	values.prepend(newVal);
	emit newValueWritten(val);
	return true;
}

ARpcISensorStorage::TimestampRule ARpcLastNValuesStorage::fixTimestampRule(ARpcISensorStorage::TimestampRule rule)
{
	if((valueType==ARpcSensor::SINGLE_LT||valueType==ARpcSensor::PACKET_LT)&&
		rule==ARpcISensorStorage::DONT_TOUCH)return ARpcISensorStorage::ADD_GT;
	return rule;
}

ARpcISensorValue* ARpcLastNValuesStorage::valueAt(quint32 index)
{
	if(index>=storedCount)return 0;
	ARpcISensorValue *rVal=mkVar();
	copyVar(values[index],rVal);
	return rVal;
}

quint32 ARpcLastNValuesStorage::valuesCount()
{
	return storedCount;
}

bool ARpcLastNValuesStorage::open()
{
	if(opened)return false;
	QFile file(dbDir.absolutePath()+"/db.index");
	if(!file.exists())return false;
	QSettings settings(dbDir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	bool ok=false;
	storedCount=settings.value("stored_count").toUInt(&ok);
	if(!ok)return false;
	if(storedCount==0)storedCount=1;
	if(!file.open(QIODevice::ReadOnly))return false;
	startIndex=file.readAll().toULong(&ok);
	file.close();
	if(!ok)return false;
	effectiveValType=defaultEffectiveValuesType(timestampRule);
	hlp=ARpcDBDriverHelpers(timestampRule);
	for(quint32 i=0;i<storedCount;++i)
	{
		ARpcISensorValue *sVal=valueFromDisk(i);
		values.append(sVal);
	}
	opened=true;
	return true;
}

void ARpcLastNValuesStorage::closeInternal()
{
	opened=false;
	storedCount=0;
	startIndex=0;
	for(quint32 i=0;i<(quint32)values.count();++i)
		delete values[i];
	values.clear();
}

void ARpcLastNValuesStorage::copyVar(const ARpcISensorValue *from,ARpcISensorValue *to)
{
	if(from->type()&ARpcSensor::singleValueFlag)
		*((ARpcSingleSensorValue*)to)=*((ARpcSingleSensorValue*)from);
	else if(from->type()&ARpcSensor::packetValueFlag)
		*((ARpcPacketSensorValue*)to)=*((ARpcPacketSensorValue*)from);
	else if(from->type()&ARpcSensor::textValueFlag)
		*((ARpcTextSensorValue*)to)=*((ARpcTextSensorValue*)from);
}

ARpcISensorValue* ARpcLastNValuesStorage::valueFromDisk(quint32 index)
{
	if(index>=storedCount)return 0;
	QFile file(dbDir.absolutePath()+"/"+QString::number((startIndex+index)%storedCount)+".data");
	if(!file.open(QIODevice::ReadOnly))return 0;
	QByteArray data=file.readAll();
	file.close();
	if(data.isEmpty())return 0;
	return hlp.unpackSensorValue(effectiveValType,data);
}

ARpcISensorValue* ARpcLastNValuesStorage::mkVar()
{
	if(effectiveValType&ARpcSensor::singleValueFlag)
		return new ARpcSingleSensorValue(0);
	else if(effectiveValType&ARpcSensor::packetValueFlag)
		return new ARpcPacketSensorValue(0);
	else if(effectiveValType&ARpcSensor::textValueFlag)
		return new ARpcTextSensorValue;
	return 0;
}

ARpcSensor::Type ARpcLastNValuesStorage::effectiveValuesType()const
{
	return effectiveValType;
}