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

#include "VDIL/blocks/LinearTransformationBlock.h"
#include "VDIL/core/CoreBlocksGroupFactory.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

const QString LinearTransformationBlock::mBlockName=QString("linear_transformation");

template<class T>
static void normCalc(const SensorValueNumeric<T> *in,SensorValueNumeric<T> *out,
	const T &minX,const T &maxX,const T &minY,const T &maxY,quint32 dimIndex,bool forceLimits)
{
	for(quint32 i=0;i<in->packetsCount();++i)
	{
		T val=minY+(in->get(dimIndex,i)-minX)*(maxY-minY)/(maxX-minX);
		if(forceLimits)
		{
			if(val<minY)
				val=minY;
			else if(val>maxY)
				val=maxY;
		}
		out->set(dimIndex,i,val);
	}
}

template<class T>
static void checkLimits(T &minX,T &maxX,T &minY,T &maxY,T addIfZeroDist)
{
	T t=qMin(minX,maxX);
	maxX=qMax(minX,maxX);
	minX=t;
	t=qMin(minY,maxY);
	maxY=qMax(minY,maxY);
	minY=t;
	if((maxX-minX)==(T)0)
		maxX=minX+addIfZeroDist;
}

LinearTransformationBlock::LinearTransformationBlock(quint32 bId)
	:BaseBlock(bId)
	,mMinX(0.0)
	,mMaxX(1.0)
	,mMinY(0.0)
	,mMaxY(1.0)
{
	mDimIndex=0;
	mForceLimits=false;
	in=mkInput(TypeConstraints(DataUnit::SINGLE|DataUnit::ARRAY,0),DataUnit::SINGLE,"in");
	out=mkOutput(TypeAndDim(DataUnit::SINGLE,1),"out");
}

QString LinearTransformationBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString LinearTransformationBlock::blockName()const
{
	return mBlockName;
}

void LinearTransformationBlock::setParams(double minX,double maxX,double minY,double maxY,quint32 dimIndex,bool forceLimits)
{
	checkLimits(minX,maxX,minY,maxY,0.001);
	mMinX=DataUnit(minX);
	mMaxX=DataUnit(maxX);
	mMinY=DataUnit(minY);
	mMaxY=DataUnit(maxY);
	mForceLimits=forceLimits;
	mDimIndex=dimIndex;
}

void LinearTransformationBlock::setParams(qint64 minX,qint64 maxX,qint64 minY,qint64 maxY,quint32 dimIndex,bool forceLimits)
{
	checkLimits(minX,maxX,minY,maxY,1LL);
	mMinX=DataUnit(minX);
	mMaxX=DataUnit(maxX);
	mMinY=DataUnit(minY);
	mMaxY=DataUnit(maxY);
	mForceLimits=forceLimits;
	mDimIndex=dimIndex;
}

const DataUnit& LinearTransformationBlock::minX()const
{
	return mMinX;
}

const DataUnit &LinearTransformationBlock::maxX()const
{
	return mMaxX;
}

const DataUnit &LinearTransformationBlock::minY()const
{
	return mMinY;
}

const DataUnit &LinearTransformationBlock::maxY()const
{
	return mMaxY;
}

quint32 LinearTransformationBlock::dimIndex()const
{
	return mDimIndex;
}

bool LinearTransformationBlock::forceLimits()const
{
	return mForceLimits;
}

void LinearTransformationBlock::eval()
{
	SensorDef::Type t=in->data().value()->type();
	if(t.dim>=mDimIndex)return;
	QScopedPointer<SensorValue> v(in->data().value()->mkCopy());
	if(in->data().numType()==DataUnit::S64&&mMinX.numType()==DataUnit::S64)
	{
		qint64 minX=mMinX.value()->valueToS64(0);
		qint64 maxX=mMaxX.value()->valueToS64(0);
		qint64 minY=mMinY.value()->valueToS64(0);
		qint64 maxY=mMaxY.value()->valueToS64(0);
		normCalc((const SensorValueS64*)in->data().value(),(SensorValueS64*)v.data(),
			minX,maxX,minY,maxY,mDimIndex,mForceLimits);
	}
	else
	{
		double minX=mMinX.value()->valueToDouble(0);
		double maxX=mMaxX.value()->valueToDouble(0);
		double minY=mMinY.value()->valueToDouble(0);
		double maxY=mMaxY.value()->valueToDouble(0);
		normCalc((const SensorValueF64*)in->data().value(),(SensorValueF64*)v.data(),
			minX,maxX,minY,maxY,mDimIndex,mForceLimits);
	}
	out->setData(DataUnit(v.data()));
}

void LinearTransformationBlock::onInputTypeSelected(BlockInput *b)
{
	Q_UNUSED(b)
	out->replaceTypeAndDim(in->type());
}
