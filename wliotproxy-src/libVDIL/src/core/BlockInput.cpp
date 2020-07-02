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

#include "VDIL/core/BlockInput.h"
#include "VDIL/core/BlockOutput.h"
#include "VDIL/core/BaseBlock.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

BlockInput::BlockInput(BaseBlock *b,TypeConstraints suppTypes,DataUnit::Type currType,const QString &title)
	:mSupportedTypes(suppTypes)
	,mCurrentType(currType)
	,mData(mCurrentType,1)
{
	mTitle=title;
	if(suppTypes.dim==0)
		mCurrentDim=1;
	else mCurrentDim=suppTypes.dim;
	mBlock=b;
	mLinkedOutput=0;
}

BlockInput::~BlockInput()
{
	if(mLinkedOutput)
		mLinkedOutput->unlinkFrom(this);
}

TypeConstraints BlockInput::supportedTypes()const
{
	return mSupportedTypes;
}

DataUnit::Type BlockInput::type()const
{
	return mCurrentType;
}

quint32 BlockInput::dim()const
{
	return mCurrentDim;
}

BaseBlock* BlockInput::block()
{
	return mBlock;
}

const BaseBlock* BlockInput::block()const
{
	return mBlock;
}

BlockOutput* BlockInput::linkedOutput()
{
	return mLinkedOutput;
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
	mDataIsSet=true;
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