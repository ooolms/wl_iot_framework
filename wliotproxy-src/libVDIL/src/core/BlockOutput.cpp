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

#include "VDIL/core/BlockOutput.h"
#include "VDIL/core/BlockInput.h"
#include "VDIL/core/BaseBlock.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

BlockOutput::BlockOutput(BaseBlock *b,TypeAndDim t,const QString &title)
{
	mBlock=b;
	mTitle=title;
	mType=t;
	if(mType.dim==0)
		mType.dim=1;
}

BlockOutput::~BlockOutput()
{
	for(BlockInput *i:linkedInputs)
		i->mLinkedOutput=nullptr;
}

TypeAndDim BlockOutput::type()const
{
	return mType;
}

bool BlockOutput::linkTo(BlockInput *input)
{
	if(linkedInputs.contains(input))
		return false;
	if(!input->supportedTypes().match(mType))
		return false;
	if(input->mLinkedOutput)
		return false;
	input->mLinkedOutput=this;
	input->mCurrentType=mType;
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

const BaseBlock* BlockOutput::block()const
{
	return mBlock;
}

void BlockOutput::replaceTypeAndDim(const TypeAndDim &t)
{
	mType=t;
	for(int i=0;i<linkedInputs.count();++i)
	{
		BlockInput *in=linkedInputs[i];
		if(in->supportedTypes().match(mType))
		{
			in->mCurrentType=mType;
			in->reset();
			in->mBlock->onInputTypeSelected(in);
		}
		else
		{
			in->mLinkedOutput=0;
			in->reset();
			linkedInputs.removeAt(i);
			--i;
		}
	}
}

void BlockOutput::setData(DataUnit u)
{
	for(BlockInput *i:linkedInputs)
		i->setData(u);
}
