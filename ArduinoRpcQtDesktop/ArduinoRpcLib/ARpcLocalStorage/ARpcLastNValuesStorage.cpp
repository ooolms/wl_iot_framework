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

//CRIT fix "values" usage !!!

ARpcLastNValuesStorage::ARpcLastNValuesStorage(const ARpcSensor &sensor,const QUuid &devId,const QString &devName,
	QObject *parent)
	:ARpcISensorStorage(sensor,devId,devName,parent)
{
	storedCount=0;
	opened=false;
	timestampRule=ADD_GT;
	effectiveValType=defaultEffectiveValuesType(timestampRule);
	dbType=FILES;
}

ARpcLastNValuesStorage::~ARpcLastNValuesStorage()
{
	close();
}

bool ARpcLastNValuesStorage::create(quint32 storedValuesCount,const ARpcISensorValue &fillerValue)
{
	if(opened)
		return false;
	if(storedValuesCount==0)
		storedValuesCount=1;
	effectiveValType=defaultEffectiveValuesType(timestampRule);
	QFile file(dbDir.absolutePath()+"/db.index");
	if(file.exists())
		return false;
	storedCount=storedValuesCount;
	if(ARpcSensor::isSingle(fillerValue.type()))
		dbType=FIXED_BLOCKS;
	else dbType=FILES;
	QSettings settings(dbDir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	settings.setValue("stored_count",QString::number(storedCount));
	settings.setValue("db_type",(dbType==FIXED_BLOCKS)?"fixed_blocks":"files");
	settings.sync();
	if(settings.status()!=QSettings::NoError)
		return false;
	if(!file.open(QIODevice::WriteOnly))
		return false;
	file.write("0");
	file.close();
	opened=true;
	values.clear();
	hlp=ARpcDBDriverHelpers(timestampRule);
	startIndex=0;
	if(dbType==FIXED_BLOCKS)
	{
		if(!dbFixesBlocks.create(dbDir.absolutePath()+"/data.db",hlp.sizesForFixedBlocksDb(fillerValue)))
			return false;
		QByteArray valData=hlp.packSensorValue(&fillerValue);
		dbFixesBlocks.addManyBlocks(storedCount,valData);
	}
	else
	{
		for(quint32 i=0;i<storedCount;++i)
			if(!writeSensorValue(&fillerValue))
				return false;
	}
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

//CRIT does'n open indexFile any time
bool ARpcLastNValuesStorage::writeSensorValue(const ARpcISensorValue *val)
{
	if(!opened)
		return false;
	QByteArray data=hlp.packSensorValue(val);
	if(data.isEmpty())
		return false;
	QFile indexFile(dbDir.absolutePath()+"/db.index");
	if(dbType==FILES)
	{
		QFile dataFile(dbDir.absolutePath()+"/"+QString::number(startIndex)+".data");
		if(!dataFile.open(QIODevice::WriteOnly)||!indexFile.open(QIODevice::WriteOnly))
			return false;
		indexFile.write(QByteArray::number((startIndex+storedCount+1)%storedCount));
		indexFile.close();
		startIndex=(startIndex+storedCount+1)%storedCount;
		if(dataFile.write(data)!=data.size())
		{
			dataFile.close();
			dataFile.remove();
		}
		else
			dataFile.close();
	}
	else
	{
		if(!indexFile.open(QIODevice::WriteOnly))
			return false;
		indexFile.write(QByteArray::number((startIndex+storedCount+1)%storedCount));
		indexFile.close();
		if(!dbFixesBlocks.updateBlock(startIndex,data))
		{
			startIndex=(startIndex+storedCount+1)%storedCount;
			return false;
		}
		else startIndex=(startIndex+storedCount+1)%storedCount;
	}
	if((quint32)values.count()>storedCount)
	{
		delete values.last();
		values.removeLast();
	}
	ARpcISensorValue *newVal=readValue(0);
	values.prepend(newVal);
	emit newValueWritten(newVal);
	return true;
}

ARpcISensorStorage::TimestampRule ARpcLastNValuesStorage::fixTimestampRule(ARpcISensorStorage::TimestampRule rule)
{
	if((mSensor.type==ARpcSensor::SINGLE_LT||mSensor.type==ARpcSensor::PACKET_LT)&&
		rule==ARpcISensorStorage::DONT_TOUCH)
		return ARpcISensorStorage::ADD_GT;
	return rule;
}

ARpcISensorValue* ARpcLastNValuesStorage::valueAt(quint64 index)
{
	if((quint32)index>=storedCount)
		return 0;
	//TODO !!!
//	ARpcISensorValue *rVal=mkVar();
//	copyVar(values[(quint32)index],rVal);
//	return rVal;
	return readValue(index);
}

quint64 ARpcLastNValuesStorage::valuesCount()
{
	return storedCount;
}

bool ARpcLastNValuesStorage::open()
{
	if(opened)
		return false;
	QFile file(dbDir.absolutePath()+"/db.index");
	if(!file.exists())
		return false;
	QSettings settings(dbDir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	bool ok=false;
	storedCount=settings.value("stored_count").toUInt(&ok);
	if(!ok)
		return false;
	if(storedCount==0)
		storedCount=1;
	if(!file.open(QIODevice::ReadOnly))
		return false;
	startIndex=file.readAll().toULong(&ok);
	file.close();
	if(!ok)
		return false;
	if(settings.value("db_type").toString()=="fixed_blocks")
		dbType=FIXED_BLOCKS;
	else dbType=FILES;
	if(dbType==FIXED_BLOCKS)
		if(!dbFixesBlocks.open(dbDir.absolutePath()+"/data.db"))return false;
	effectiveValType=defaultEffectiveValuesType(timestampRule);
	hlp=ARpcDBDriverHelpers(timestampRule);
	for(quint32 i=0;i<storedCount;++i)
	{
		ARpcISensorValue *sVal=readValue(i);
		if(sVal)values.append(sVal);
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
	if(dbType==FIXED_BLOCKS)
		dbFixesBlocks.close();
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

ARpcISensorValue* ARpcLastNValuesStorage::readValue(quint32 index)
{
	if(index>=storedCount)
		return 0;
	QByteArray data;
	if(dbType==FILES)
	{
		QFile file(dbDir.absolutePath()+"/"+QString::number((startIndex+index)%storedCount)+".data");
		if(!file.open(QIODevice::ReadOnly))
			return 0;
		data=file.readAll();
		file.close();
		if(data.isEmpty())
			return 0;
	}
	else if(!dbFixesBlocks.readBlock(index,data))return 0;
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

ARpcSensor::Type ARpcLastNValuesStorage::effectiveValuesType() const
{
	return effectiveValType;
}
