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

#include "VDIL/blocks/StorageSourceBlock.h"
#include "VDIL/core/Program.h"
#include "VDIL/core/CoreBlocksGroupFactory.h"
#include "wliot/storages/ISensorStorage.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

const QString StorageSourceBlock::mBlockName=QString("storage_source");

StorageSourceBlock::StorageSourceBlock(quint32 bId)
	:SourceBlock(bId)
{
	mValType=SensorDef::Type(SensorDef::BAD_TYPE,SensorDef::SINGLE,SensorDef::NO_TIME,1);
	mCount=0;
	mNeedDevice=false;
	mUseTriggger=true;
}

DataUnit StorageSourceBlock::extractDataInternal()
{
	if(!helper()||mCount==0)
		return DataUnit();
	if(mNeedDevice&&!helper()->devIsConnected(mStorId.deviceId))
		return DataUnit();
	ISensorStorage *stor=helper()->storageById(mStorId);
	if(!stor||stor->storedValuesType()!=mValType)
		return DataUnit();
	stor->open();
	if(stor->valuesCount()<mCount)
		return DataUnit();
	if(mCount==1||stor->storedValuesType().packType==SensorDef::PACKET)
	{
		QScopedPointer<SensorValue> v(stor->valueAt(stor->valuesCount()-1));
		return DataUnit(v.data(),true);
	}
	else
	{
		QVector<SensorValue*> vals;
		vals.resize(mCount);
		for(quint64 i=0;i<mCount;++i)
		{
			SensorValue *v=stor->valueAt(stor->valuesCount()-i);
			if(v)vals.append(v);
		}
		if(vals.isEmpty())
			return DataUnit();
		DataUnit v=DataUnit(vals);
		for(SensorValue *v:vals)
			delete v;
	}
}

QString StorageSourceBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString StorageSourceBlock::blockName()const
{
	return mBlockName;
}

quint32 StorageSourceBlock::count()const
{
	return mCount;
}

const StorageId &StorageSourceBlock::storageId()const
{
	return mStorId;
}

const SensorDef::Type& StorageSourceBlock::valuesType()const
{
	return mValType;
}

bool StorageSourceBlock::needDevice()const
{
	return mNeedDevice;
}

bool StorageSourceBlock::useTrigger()const
{
	return mUseTriggger;
}

void StorageSourceBlock::setParams(StorageId stId,SensorDef::Type valType,
	quint32 cnt,bool needDevice,bool useTrigger)
{
	mCount=cnt;
	mNeedDevice=needDevice;
	mUseTriggger=useTrigger;
	if(mCount==0||valType.packType==SensorDef::PACKET)
		mCount=1;
	mStorId=stId;
	mValType=valType;
	DataUnit::Type outType=DataUnit::typeForSensorValue(mValType,mCount==1);
	if(!mStorId.deviceId.isNull()&&!mStorId.sensorName.isEmpty()&&mValType.isValid()&&outType!=DataUnit::INVALID)
	{
		if(outputsCount()>0)
			output(0)->replaceTypeAndDim(TypeAndDim(outType,mValType.dim));
		else mkOutput(TypeAndDim(outType,mValType.dim),"out");
	}
	else
	{
		if(outputsCount()>0)
			rmOutput(0);
	}
}

QList<QUuid> StorageSourceBlock::usedDevices()const
{
	QList<QUuid> l;
	if(!mStorId.deviceId.isNull())
		l.append(mStorId.deviceId);
	return l;
}
