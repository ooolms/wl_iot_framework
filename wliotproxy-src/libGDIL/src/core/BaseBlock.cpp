#include "GDIL/core/BaseBlock.h"

BaseBlock::BaseBlock(quint32 bId)
{
	prg=0;
	mBlockId=bId;
}

BaseBlock::~BaseBlock()
{
	for(BlockInput *i:inputs)
		delete i;
	for(BlockOutput *o:outputs)
		delete o;
}

int BaseBlock::inputsCount()
{
	return inputs.count();
}

BlockInput* BaseBlock::input(int index)
{
	return inputs.value(index);
}

int BaseBlock::outputsCount()
{
	return outputs.count();
}

BlockOutput* BaseBlock::output(int index)
{
	return outputs.value(index);
}

int BaseBlock::outputIndex(const BlockOutput *out)const
{
	return outputs.indexOf(const_cast<BlockOutput*>(out));
}

int BaseBlock::inputIndex(const BlockInput *in)const
{
	return inputs.indexOf(const_cast<BlockInput*>(in));
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

void BaseBlock::onInputTypeSelected(BlockInput *b)
{
	Q_UNUSED(b)
}
