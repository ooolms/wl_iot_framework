#include "GIDL/core/BaseBlock.h"

BaseBlock::BaseBlock(quint32 id)
{
	mId=id;
}

BaseBlock::~BaseBlock()
{
}

int BaseBlock::inputsCount()
{
	return inputs.count();
}

BlockInput* BaseBlock::input(int index)
{
	return inputs[index];
}

int BaseBlock::outputsCount()
{
	return outputs.count();
}

BlockOutput* BaseBlock::ountput(int index)
{
	return outputs[index];
}

quint32 BaseBlock::id()
{
	return mId;
}

void BaseBlock::evalIfReady()
{
	bool hasEmptyInput=false;
	for(int i=0;i<inputs.count();++i)
	{
		if(!inputs[i]->isReady())
		{
			hasEmptyInput=true;
			break;
		}
	}
	if(!hasEmptyInput)
		eval();
}
