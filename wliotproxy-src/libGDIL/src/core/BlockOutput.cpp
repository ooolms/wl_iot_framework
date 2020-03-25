#include "GIDL/core/BlockOutput.h"
#include "GIDL/core/BlockInput.h"
#include "GIDL/core/BaseBlock.h"

BlockOutput::BlockOutput(DataUnit::Type t,quint32 dim)
{
	mType=t;
	if(dim==0)
		dim=1;
	mDim=dim;
}

BlockOutput::~BlockOutput()
{
	for(BlockInput *i:linkedInputs)
		i->mLinkedOutput=nullptr;
}

DataUnit::Type BlockOutput::outputType()const
{
	return mType;
}

bool BlockOutput::linkTo(BlockInput *input)
{
	if(linkedInputs.contains(input))
		return false;
	if(!input->canConnectType(mType,mDim))
		return false;
	input->mLinkedOutput=this;
	input->mCurrentType=mType;
	input->mCurrentDim=mDim;
	input->reset();
	input->mBlock->onInputTypeSelected(input);
	linkedInputs.insert(input);
	return true;
}

void BlockOutput::setData(DataUnit u)
{
	for(BlockInput *i:linkedInputs)
		i->setData(u);
}
