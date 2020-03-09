#include "GIDL/core/BlockInput.h"
#include "GIDL/core/BlockOutput.h"
#include "GIDL/core/BaseBlock.h"

BlockInput::BlockInput(BaseBlock *b,DataUnit::Types suppTypes,DataUnit::Type currType)
	:mSupportedTypes(suppTypes)
	,mCurrentType(currType)
	,mData(mCurrentType)
{
	mBlock=b;
	mLinkedOutput=0;
}

BlockInput::~BlockInput()
{
	if(mLinkedOutput)
		mLinkedOutput->unlinkFrom(this);
}

DataUnit::Type BlockInput::currentType()const
{
	return mCurrentType;
}

bool BlockInput::canConnectType(DataUnit::Type t)const
{
	return mSupportedTypes.testFlag(t);
}

void BlockInput::reset()
{
	mDataIsSet=false;
	mData=DataUnit(mCurrentType);
}

bool BlockInput::setData(DataUnit u)
{
	if(u.type()!=mCurrentType)
		return false;
	mData=u;
	mBlock->evalIfReady();
	return true;
}

bool BlockInput::isReady()
{
	return mDataIsSet;
}
