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

#include "GDIL/core/BlockOutput.h"
#include "GDIL/core/BlockInput.h"
#include "GDIL/core/BaseBlock.h"

using namespace WLIOT;
using namespace WLIOTGDIL;

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
	if(!input->supportedTypes().match(mType,mDim))
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

void BlockOutput::replaceTypeAndDim(DataUnit::Type newType,quint32 newDim)
{
	mType=newType;
	mDim=newDim;
	for(int i=0;i<linkedInputs.count();++i)
	{
		BlockInput *in=linkedInputs[i];
		if(in->supportedTypes().match(mType,mDim))
		{
			in->mCurrentType=mType;
			in->mCurrentDim=mDim;
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
