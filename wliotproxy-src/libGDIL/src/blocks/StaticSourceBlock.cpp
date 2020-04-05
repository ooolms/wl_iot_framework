#include "GDIL/blocks/StaticSourceBlock.h"

const QUuid StaticSourceBlock::mTypeId=QUuid("{0fa7a81b-9240-424f-a83d-eec12d568a47}");

StaticSourceBlock::StaticSourceBlock(quint32 bId)
	:SourceBlock(bId)
	,mValue(0.0)
{
	if(mValue.isValid())
		setOutput(new BlockOutput(this,mValue.type(),mValue.dim(),"out"));
}

const DataUnit& StaticSourceBlock::value()const
{
	return mValue;
}

void StaticSourceBlock::setValue(const DataUnit &u)
{
	mValue=u;
	if(mValue.isValid())
		setOutput(new BlockOutput(this,mValue.type(),mValue.dim(),"out"));
	else setOutput(0);
	if(mValue.type()==DataUnit::BOOL)
		hint=mValue.value()->valueToS64(0)==1?"true":"false";
	else hint=mValue.value()->dumpToMsgArgs().join("|");
}

QUuid StaticSourceBlock::typeId()const
{
	return mTypeId;
}

DataUnit StaticSourceBlock::extractDataInternal()
{
	return mValue;
}
