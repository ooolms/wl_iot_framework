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

#include "wliot/storages/LastNValuesStorage.h"
#include "FSSensorStorageHelper.h"
#include "DBDriverHelpers.h"
#include "DBDriverFixedBlocks.h"
#include <QDateTime>

LastNValuesStorage::LastNValuesStorage(const QString &path,const QUuid &devId,const QByteArray &devName,
	const SensorDef &sensor,TimestampRule tsRule,QObject *parent)
	:BaseFSSensorStorage(path,devId,devName,sensor,LAST_N_VALUES,tsRule,parent)
{
	storedCount=0;
	opened=false;
	dbType=FILES;
	dbFixesBlocks=new DBDriverFixedBlocks;
}

LastNValuesStorage::~LastNValuesStorage()
{
	close();
	delete dbFixesBlocks;
}

bool LastNValuesStorage::create(quint32 storedValuesCount)
{
	if(opened)
		return false;
	if(storedValuesCount==0)
		storedValuesCount=1;
	QFile file(fsStorageHelper->dbPath()+"/db.index");
	if(file.exists())
		return false;
	storedCount=storedValuesCount;
	if(mSensor.type.hasFixedSize())
		dbType=FIXED_BLOCKS;
	else dbType=FILES;
	fsStorageHelper->settings()->setValue("stored_count",QString::fromUtf8(QByteArray::number(storedCount)));
	fsStorageHelper->settings()->setValue("db_type",(dbType==FIXED_BLOCKS)?"fixed_blocks":"files");
	fsStorageHelper->settings()->sync();
	if(fsStorageHelper->settings()->status()!=QSettings::NoError)
		return false;
	if(!file.open(QIODevice::WriteOnly))
		return false;
	file.write("0");
	file.close();
	opened=true;
	startIndex=0;
	QScopedPointer<SensorValue>templateValue(SensorValue::createSensorValue(mStoredValuesType));
	QByteArray valData=hlp->packSensorValue(templateValue.data());
	values.clear();
	for(quint32 i=0;i<storedCount;++i)
		values.append(valData);
	if(dbType==FIXED_BLOCKS)
	{
		if(!dbFixesBlocks->create(fsStorageHelper->dbPath()+"/data.db",
			DBDriverHelpers::sizesForFixedBlocksDb(mStoredValuesType)))
			return false;
		dbFixesBlocks->addManyBlocks(storedCount,valData);
	}
	else
	{
		for(quint32 i=0;i<storedCount;++i)
			if(!writeSensorValue(templateValue.data()))
				return false;
	}
	return true;
}

bool LastNValuesStorage::isOpened()const
{
	return opened;
}

//TODO does'n open indexFile any time
bool LastNValuesStorage::writeSensorValue(const SensorValue *val)
{
	if(!opened)
		return false;
	QByteArray data=hlp->packSensorValue(val);
	if(data.isEmpty())
		return false;
	QFile indexFile(fsStorageHelper->dbPath()+"/db.index");
	if(dbType==FILES)
	{
		QFile dataFile(fsStorageHelper->dbPath()+"/"+QString::fromUtf8(QByteArray::number(startIndex))+".data");
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
		if(!dbFixesBlocks->updateBlock(startIndex,data))
		{
			startIndex=(startIndex+storedCount+1)%storedCount;
			return false;
		}
		else startIndex=(startIndex+storedCount+1)%storedCount;
	}
	values.removeFirst();
	values.append(data);
	SensorValue *newVal=readValue(storedCount-1);
	emit newValueWritten(newVal);
	return true;
}

SensorValue* LastNValuesStorage::valueAt(quint64 index)
{
	if((quint32)index>=storedCount)
		return 0;
	return hlp->unpackSensorValue(mStoredValuesType,values[(quint32)index]);
}

quint64 LastNValuesStorage::valuesCount()
{
	return storedCount;
}

bool LastNValuesStorage::open()
{
	if(opened)
		return false;
	QFile file(fsStorageHelper->dbPath()+"/db.index");
	if(!file.exists())
		return false;
	bool ok=false;
	storedCount=fsStorageHelper->settings()->value("stored_count").toString().toUtf8().toUInt(&ok);
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
	if(fsStorageHelper->settings()->value("db_type").toString()=="fixed_blocks")
		dbType=FIXED_BLOCKS;
	else dbType=FILES;
	if(dbType==FIXED_BLOCKS)
		if(!dbFixesBlocks->open(fsStorageHelper->dbPath()+"/data.db"))return false;
	for(quint32 i=0;i<storedCount;++i)
		values.append(readValueData(i));
	opened=true;
	return true;
}

void LastNValuesStorage::close()
{
	opened=false;
	storedCount=0;
	startIndex=0;
	values.clear();
	if(dbType==FIXED_BLOCKS)
		dbFixesBlocks->close();
}

SensorValue* LastNValuesStorage::readValue(quint32 index)
{
	return hlp->unpackSensorValue(mStoredValuesType,readValueData(index));
}

QByteArray LastNValuesStorage::readValueData(quint32 index)
{
	if(index>=storedCount)
		return QByteArray();
	QByteArray data;
	if(dbType==FILES)
	{
		QFile file(fsStorageHelper->dbPath()+"/"+
			QString::fromUtf8(QByteArray::number((startIndex+index)%storedCount))+".data");
		if(!file.open(QIODevice::ReadOnly))
			return QByteArray();
		data=file.readAll();
		file.close();
		if(data.isEmpty())
			return QByteArray();
	}
	else if(!dbFixesBlocks->readBlock((startIndex+index)%storedCount,data))return QByteArray();
	return data;
}
