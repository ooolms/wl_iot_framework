#include "GIDL/core/BaseBlock.h"

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
