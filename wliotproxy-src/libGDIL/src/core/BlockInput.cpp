#include "GIDL/core/BlockInput.h"
#include "GIDL/core/BlockOutput.h"
#include "GIDL/core/BaseBlock.h"

BlockInput::BlockInput(BaseBlock *b,DataUnit::Types suppTypes,DataUnit::Type currType,quint32 supportedDim)
	:mSupportedTypes(suppTypes)
	,mCurrentType(currType)
	,mData(mCurrentType,1)
{
	mSupportedDim=supportedDim;
	if(mSupportedDim==0)
		mCurrentDim=1;
	else mCurrentDim=mSupportedDim;
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

quint32 BlockInput::supportedDim()const
{
	return mSupportedDim;
}

quint32 BlockInput::currentDim()const
{
	return mCurrentDim;
}

bool BlockInput::canConnectType(DataUnit::Type t,quint32 dim)const
{
	if(mSupportedDim!=0&&mSupportedDim!=dim)
		return false;
	return mSupportedTypes.testFlag(t);
}

void BlockInput::reset()
{
	mDataIsSet=false;
	mData=DataUnit(mCurrentType,mCurrentDim);
}

bool BlockInput::setData(DataUnit u)
{
	if(u.type()!=mCurrentType||u.dim()!=mCurrentDim)
		return false;
	mData=u;
	mBlock->evalIfReady();
	return true;
}

bool BlockInput::isReady()
{
	return mDataIsSet;
}

const DataUnit& BlockInput::data()
{
	return mData;
}
