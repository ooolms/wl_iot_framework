#include "GIDL/blocks/StaticSourceBlock.h"

StaticSourceBlock::StaticSourceBlock(const DataUnit &t,quint32 bId)
	:SourceBlock(bId)
	,mStaticData(t)
{
	if(mStaticData.isValid())
		setOutput(new BlockOutput(mStaticData.type(),mStaticData.dim()));
}

const DataUnit& StaticSourceBlock::data()const
{
	return mStaticData;
}

void StaticSourceBlock::setData(const DataUnit &u)
{
	mStaticData=u;
	if(mStaticData.isValid())
		setOutput(new BlockOutput(mStaticData.type(),mStaticData.dim()));
	else setOutput(0);
}

DataUnit StaticSourceBlock::extractDataInternal()
{
	return mStaticData;
}
