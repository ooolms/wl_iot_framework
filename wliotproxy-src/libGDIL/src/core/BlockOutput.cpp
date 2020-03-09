#include "GIDL/core/BlockOutput.h"
#include "GIDL/core/BlockInput.h"

BlockOutput::BlockOutput()
{
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
	if(!input->canConnectType(mType))
		return false;
	input->mLinkedOutput=this;
	input->mCurrentType=mType;
	input->reset();
	linkedInputs.insert(input);
	return true;
}

void BlockOutput::setData(DataUnit u)
{
	for(BlockInput *i:linkedInputs)
		i->setData(u);
}
