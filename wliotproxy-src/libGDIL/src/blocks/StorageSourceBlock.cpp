#include "GIDL/blocks/StorageSourceBlock.h"

StorageSourceBlock::StorageSourceBlock(ISensorStorage *stor,StorageId stId,quint32 bId,quint32 cnt)
	:SourceBlock(bId)
{
	mStor=stor;
	mStorId=stId;
	mCount=cnt;
	if(mCount==0)
		mCount=1;
	if(mStor)
	{
		setOutput(new BlockOutput(DataUnit::typeForSensorValue(mStor->storedValuesType(),mCount==1),
			mStor->storedValuesType().dim));
	}
}

DataUnit StorageSourceBlock::extractDataInternal()
{
	if(!mStor)
		return DataUnit(DataUnit::INVALID,1);
	mStor->open();
	if(mStor->valuesCount()<mCount)
		return DataUnit(DataUnit::INVALID,1);
	if(mCount==1||mStor->storedValuesType().packType==SensorDef::PACKET)
	{
		QScopedPointer<SensorValue> v(mStor->valueAt(mStor->valuesCount()-1));
		return DataUnit(v.data());
	}
	else
	{
		QVector<SensorValue*> vals;
		vals.resize(mCount);
		for(quint64 i=0;i<mCount;++i)
			vals.append(mStor->valueAt(mStor->valuesCount()-i));
		return DataUnit(vals);
	}
}

quint32 StorageSourceBlock::count()const
{
	return mCount;
}

void StorageSourceBlock::setCount(quint32 c)
{
	if(c==0)c=1;
	if(mCount==c)return;
	bool updateOutput=(mCount==1||c==1);
	mCount=c;
	if(updateOutput)
	{
		if(mStor)
		{
			setOutput(new BlockOutput(DataUnit::typeForSensorValue(mStor->storedValuesType(),mCount==1),
				mStor->storedValuesType().dim));
		}
		else setOutput(0);
	}
}
