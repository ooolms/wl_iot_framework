#include "GDIL/core/BlockOutput.h"
#include "GDIL/core/BlockInput.h"
#include "GDIL/core/BaseBlock.h"

BlockOutput::BlockOutput(BaseBlock *b,DataUnit::Type t,quint32 dim,const QString &title)
{
	mBlock=b;
	mTitle=title;
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

DataUnit::Type BlockOutput::type()const
{
	return mType;
}

quint32 BlockOutput::dim()const
{
	return mDim;
}

bool BlockOutput::linkTo(BlockInput *input)
{
	if(linkedInputs.contains(input))
		return false;
	if(!input->canConnectType(mType,mDim))
		return false;
	if(input->mLinkedOutput)
		return false;
	input->mLinkedOutput=this;
	input->mCurrentType=mType;
	input->mCurrentDim=mDim;
	input->reset();
	input->mBlock->onInputTypeSelected(input);
	linkedInputs.append(input);
	return true;
}

void BlockOutput::unlinkFrom(BlockInput *input)
{
	if(!linkedInputs.contains(input))return;
	linkedInputs.removeOne(input);
	input->mLinkedOutput=0;
	input->reset();
}

int BlockOutput::linksCount()const
{
	return linkedInputs.count();
}

const BlockInput* BlockOutput::link(int index)const
{
	return linkedInputs.value(index);
}

BaseBlock* BlockOutput::block()
{
	return mBlock;
}

void BlockOutput::setData(DataUnit u)
{
	for(BlockInput *i:linkedInputs)
		i->setData(u);
}
