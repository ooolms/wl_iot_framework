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

#include "ARpcContinuousStorage.h"
#include "ARpcDBDriverHelpers.h"
#include <QDir>
#include <QDateTime>
#include <QSettings>

ARpcContinuousStorage::ARpcContinuousStorage(const QString &path,const QUuid &devId,const QByteArray &devName,
	const ARpcSensorDef &sensor,TimestampRule tsRule,QObject *parent)
	:ARpcBaseFSSensorStorage(path,devId,devName,sensor,CONTINUOUS,tsRule,parent)
{
	fbDb=new ARpcDBDriverFixedBlocks(this);
	cbDb=new ARpcDBDriverChainedBlocks(this);
	indDb=new ARpcDBDriverGTimeIndex(this);
	opened=false;
	mHasGTIndex=false;
	dbType=CHAINED_BLOCKS;
}

ARpcContinuousStorage::~ARpcContinuousStorage()
{
	close();
}

bool ARpcContinuousStorage::writeSensorValue(const ARpcSensorValue *val)
{
	if(!opened)
		return false;
	if(val->type()!=mSensor.type)
		return false;
	int hasTime;
	qint64 ts;
	QByteArray data=hlp.packSensorValue(val,hasTime,ts);
	if(data.isEmpty())
		return false;
	if(dbType==FIXED_BLOCKS)
	{
		if(fbDb->writeBlock(data))
		{
			if(mHasGTIndex)
				indDb->append(ts,fbDb->blocksCount()-1);
			QScopedPointer<ARpcSensorValue> valWritten(hlp.unpackSensorValue(mStoredValuesType,data));
			if(valWritten.data())
				emit newValueWritten(valWritten.data());
			return true;
		}
		return false;
	}
	else
	{
		if(cbDb->writeBlock(data))
		{
			if(mHasGTIndex)
				indDb->append(ts,cbDb->blocksCount()-1);
			QScopedPointer<ARpcSensorValue> valWritten(hlp.unpackSensorValue(mStoredValuesType,data));
			if(valWritten.data())
				emit newValueWritten(valWritten.data());
			return true;
		}
		return false;
	}
}

bool ARpcContinuousStorage::createAsFixedBlocksDb(bool gtIndex)
{
	if(opened)
		return false;
	if(!fbDb->create(fsStorageHelper.dbPath()+"/data.db",
		ARpcDBDriverHelpers::sizesForFixedBlocksDb(mStoredValuesType)))
		return false;
	mHasGTIndex=gtIndex&&(mStoredValuesType.tsType==ARpcSensorDef::GLOBAL_TIME);
	if(mHasGTIndex)
	{
		if(!indDb->create(fsStorageHelper.dbPath()+"/index.db"))
			return false;
	}
	fsStorageHelper.settings()->setValue("db_type","fixed_blocks");
	fsStorageHelper.settings()->setValue("gt_index",mHasGTIndex?"1":"0");
	fsStorageHelper.settings()->sync();
	dbType=FIXED_BLOCKS;
	opened=true;
	return true;
}

bool ARpcContinuousStorage::createAsChainedBlocksDb(bool gtIndex)
{
	if(opened)
		return false;
	if(!cbDb->create(fsStorageHelper.dbPath()+"/data.db"))
		return true;
	mHasGTIndex=gtIndex&&(mStoredValuesType.tsType==ARpcSensorDef::GLOBAL_TIME);
	if(mHasGTIndex)
	{
		if(!indDb->create(fsStorageHelper.dbPath()+"/index.db"))
			return false;
	}
	fsStorageHelper.settings()->setValue("db_type","chained_blocks");
	fsStorageHelper.settings()->setValue("gt_index",mHasGTIndex?"1":"0");
	fsStorageHelper.settings()->sync();
	dbType=CHAINED_BLOCKS;
	opened=true;
	return true;
}

void ARpcContinuousStorage::close()
{
	if(!opened)
		return;
	if(dbType==FIXED_BLOCKS)
		fbDb->close();
	else
		cbDb->close();
	opened=false;
}

quint64 ARpcContinuousStorage::valuesCount()
{
	if(!opened)
		return 0;
	if(dbType==FIXED_BLOCKS)
		return fbDb->blocksCount();
	else
		return cbDb->blocksCount();
}

ARpcSensorValue *ARpcContinuousStorage::valueAt(quint64 index)
{
	if(!opened)
		return 0;
	QByteArray data;
	if(dbType==FIXED_BLOCKS&&!fbDb->readBlock(index,data))
		return 0;
	else if(dbType==CHAINED_BLOCKS&&!cbDb->readBlock((quint32)index,data))
		return 0;
	return hlp.unpackSensorValue(mStoredValuesType,data);
}

bool ARpcContinuousStorage::hasGTIndex()
{
	return opened&&mHasGTIndex;
}

quint64 ARpcContinuousStorage::findInGTIndex(qint64 ts)
{
	if(!opened||!mHasGTIndex)
		return 0;
	return indDb->findIndex(ts);
}

bool ARpcContinuousStorage::create(bool gtIndex)
{
	if(mSensor.type.hasFixedSize())
		return createAsFixedBlocksDb(gtIndex);
	else return createAsChainedBlocksDb(gtIndex);
}

bool ARpcContinuousStorage::isOpened() const
{
	return opened;
}

bool ARpcContinuousStorage::open()
{
	if(opened)
		return false;
	QString dbTypeStr=fsStorageHelper.settings()->value("db_type").toString();
	if(dbTypeStr=="fixed_blocks")
	{
		if(!fbDb->open(fsStorageHelper.dbPath()+"/data.db"))
			return false;
		dbType=FIXED_BLOCKS;
	}
	else if(dbTypeStr=="chained_blocks")
	{
		if(!cbDb->open(fsStorageHelper.dbPath()+"/data.db"))
			return false;
		dbType=CHAINED_BLOCKS;
	}
	else
		return false;
	mHasGTIndex=(fsStorageHelper.settings()->value("gt_index").toString()=="1");
	if(mHasGTIndex)
	{
		if(!indDb->open(fsStorageHelper.dbPath()+"/index.db"))
			return false;
	}
	opened=true;
	return true;
}
