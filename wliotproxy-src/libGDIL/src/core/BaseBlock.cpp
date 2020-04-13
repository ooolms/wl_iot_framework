/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

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

bool BaseBlock::isSourceBlock()const
{
	return false;
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

quint32 BaseBlock::blockId()const
{
	return mBlockId;
}

int BaseBlock::inputIndex(const BlockInput *in)const
{
	return inputs.indexOf(const_cast<BlockInput*>(in));
}

void BaseBlock::evalIfReady()
{
	bool hasEmptyInput=false;
	for(BlockInput *i:inputs)
	{
		if(!i->isReady())
		{
			hasEmptyInput=true;
			break;
		}
	}
	if(!hasEmptyInput)
	{
		eval();
		for(BlockInput *i:inputs)
			i->reset();
	}
}

void BaseBlock::onInputTypeSelected(BlockInput *b)
{
	Q_UNUSED(b)
}

BlockInput* BaseBlock::mkInput(DataUnit::Types suppTypes,
	DataUnit::Type currType,quint32 supportedDim,const QString &title)
{
	BlockInput *in=new BlockInput(this,suppTypes,currType,supportedDim,title);
	inputs.append(in);
	return in;
}

BlockOutput *BaseBlock::mkOutput(DataUnit::Type type,quint32 dim,const QString &title)
{
	BlockOutput *out=new BlockOutput(this,type,dim,title);
	outputs.append(out);
	return out;
}

void BaseBlock::rmInput(int index)
{
	if(index<0||index>=inputs.count())
		return;
	delete inputs.takeAt(index);
}

void BaseBlock::rmInput(BlockInput *in)
{
	rmInput(inputs.indexOf(in));
}

void BaseBlock::rmOutput(int index)
{
	if(index<0||index>=outputs.count())
		return;
	delete outputs.takeAt(index);
}

void BaseBlock::rmOutput(BlockOutput *out)
{
	rmOutput(outputs.indexOf(out));
}
