#include "GDIL/blocks/StorageSourceBlock.h"
#include "GDIL/core/Program.h"

const QUuid StorageSourceBlock::mTypeId=QUuid("{b7ea1fc3-8243-4d72-8ece-25ceb10b2962}");

StorageSourceBlock::StorageSourceBlock(quint32 bId)
	:SourceBlock(bId)
{
	mValType=SensorDef::Type(SensorDef::BAD_TYPE,SensorDef::SINGLE,SensorDef::NO_TIME,1);
	mCount=0;
}

DataUnit StorageSourceBlock::extractDataInternal()
{
	if(!prg->helper()||mCount==0)
		return DataUnit(DataUnit::INVALID,1);
	ISensorStorage *stor=prg->helper()->storageById(mStorId);
	if(!stor||stor->storedValuesType()!=mValType)
		return DataUnit(DataUnit::INVALID,1);
	stor->open();
	if(stor->valuesCount()<mCount)
		return DataUnit(DataUnit::INVALID,1);
	if(mCount==1||stor->storedValuesType().packType==SensorDef::PACKET)
	{
		QScopedPointer<SensorValue> v(stor->valueAt(stor->valuesCount()-1));
		return DataUnit(v.data());
	}
	else
	{
		QVector<SensorValue*> vals;
		vals.resize(mCount);
		for(quint64 i=0;i<mCount;++i)
			vals.append(stor->valueAt(stor->valuesCount()-i));
		return DataUnit(vals);
	}
}

quint32 StorageSourceBlock::count()const
{
	return mCount;
}

const StorageId &StorageSourceBlock::storageId() const
{
	return mStorId;
}

const QString &StorageSourceBlock::devName() const
{
	return mDevName;
}

const SensorDef::Type StorageSourceBlock::valuesType() const
{
	return mValType;
}

void StorageSourceBlock::setParams(StorageId stId,const QString &devName,SensorDef::Type valType,quint32 cnt)
{
	mCount=cnt;
	if(mCount==0)
		mCount=1;
	mStorId=stId;
	mDevName=devName;
	mValType=valType;
	if(!mStorId.deviceId.isNull()&&!mStorId.sensorName.isEmpty()&&mValType.isValid())
		setOutput(new BlockOutput(this,DataUnit::typeForSensorValue(mValType,mCount==1),mValType.dim,"out"));
	else setOutput(0);
}

QUuid StorageSourceBlock::typeId()const
{
	return mTypeId;
}
