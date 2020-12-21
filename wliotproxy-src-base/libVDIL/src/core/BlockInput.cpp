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

BlockInput::BlockInput(BaseBlock *b, TypeConstraints suppTypes,TypeAndDim currType,const QString &title)
{
	mSupportedTypes=suppTypes;
	mTitle=title;
	if(suppTypes.match(currType))
		mCurrentType=currType;
	else mCurrentType=TypeAndDim(DataUnit::INVALID,1);
	mBlock=b;
	mLinkedOutput=0;
	reset();
}

BlockInput::BlockInput(BaseBlock *b,const TypeAndDim &fixedType,const QString &title)
{
	mSupportedTypes=TypeConstraints(fixedType.type,fixedType.dim);
	mTitle=title;
	mCurrentType=fixedType;
	mData=DataUnit(mCurrentType);
	mBlock=b;
	mLinkedOutput=0;
	reset();
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

TypeAndDim BlockInput::type()const
{
	return mCurrentType;
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

void BlockInput::replaceTypesAndDim(TypeConstraints suppTypes,TypeAndDim currType)
{
	mSupportedTypes=suppTypes;
	if(mLinkedOutput)
	{
		if(mSupportedTypes.match(mLinkedOutput->type()))
		{
			mCurrentType=mLinkedOutput->type();
			reset();
			mBlock->onInputTypeSelected(this);
			return;
		}
		else mLinkedOutput->unlinkFrom(this);
	}
	if(mSupportedTypes.match(currType))
		mCurrentType=currType;
	else mCurrentType=TypeAndDim(DataUnit::INVALID,1);
	reset();
	mBlock->onInputTypeSelected(this);
}

void BlockInput::replaceTypesAndDim(const TypeAndDim &fixedType)
{
	replaceTypesAndDim(TypeConstraints(fixedType),fixedType);
}

void BlockInput::addLinePoint(int index,const QPointF &p)
{
	if(mLinkedOutput)
		mLinePoints.insert(index,p);
}

void BlockInput::rmLinePoint(int index)
{
	if(mLinkedOutput)
		mLinePoints.removeAt(index);
}

void BlockInput::setLinePoint(int index,const QPointF &p)
{
	if(mLinkedOutput&&mLinePoints.count()>index)
		mLinePoints[index]=p;
}

void BlockInput::setLinePoints(const QList<QPointF> &pts)
{
	if(mLinkedOutput)
		mLinePoints=pts;
}

const QList<QPointF> BlockInput::linePoints()const
{
	return mLinePoints;
}

void BlockInput::reset()
{
	mDataIsSet=false;
	mData=DataUnit(mCurrentType);
}

bool BlockInput::setData(DataUnit u)
{
	if(!(u.typeAndDim()==mCurrentType))
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
