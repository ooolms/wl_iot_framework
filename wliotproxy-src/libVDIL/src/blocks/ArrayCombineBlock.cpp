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

#include "VDIL/blocks/ArrayCombineBlock.h"
#include "VDIL/core/CoreBlocksGroupFactory.h"
#include "wliot/devices/SensorValue.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

const QString ArrayCombineBlock::mBlockName=QString("array_combine");

ArrayCombineBlock::ArrayCombineBlock(quint32 bId)
	:BaseBlock(bId)
{
	mDim=1;
	mArrayInputs.append(mkInput(TypeConstraints(DataUnit::SINGLE|DataUnit::ARRAY,mDim),DataUnit::SINGLE,"in 1"));
	out=mkOutput(DataUnit::ARRAY,mDim,"out");
}

QString ArrayCombineBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString ArrayCombineBlock::blockName()const
{
	return mBlockName;
}

void ArrayCombineBlock::setParams(quint32 inCount,quint32 dim)
{
	if(mDim!=dim)
	{
		while(!mArrayInputs.isEmpty())
		{
			rmInput(mArrayInputs.last());
			mArrayInputs.removeLast();
		}
		mDim=dim;
		if(mDim==0)
			mDim=1;
		out->replaceTypeAndDim(DataUnit::ARRAY,mDim);
	}
	quint32 currInCount=mArrayInputs.count();
	if(inCount<currInCount)
		for(quint32 i=inCount;i<currInCount;++i)
		{
			rmInput(mArrayInputs.last());
			mArrayInputs.removeLast();
		}
	else if(inCount>currInCount)
		for(quint32 i=currInCount;i<inCount;++i)
			mArrayInputs.append(mkInput(TypeConstraints(DataUnit::SINGLE|DataUnit::ARRAY,mDim),
				DataUnit::SINGLE,"in "+QString::number(i+1)));
}

quint32 ArrayCombineBlock::inputsCount()const
{
	return mArrayInputs.count();
}

quint32 ArrayCombineBlock::dim()const
{
	return mDim;
}

void ArrayCombineBlock::eval()
{
	if(mArrayInputs.isEmpty())return;
	bool allS64=true;
	for(BlockInput *i:mArrayInputs)
	{
		if(i->data().numType()!=DataUnit::S64)
		{
			allS64=false;
			break;
		}
	}
	if(allS64)
	{
		QVector<qint64> values;
		for(BlockInput *i:mArrayInputs)
		{
			DataUnit v=i->data();
			values.append(((const SensorValueS64*)v.value())->data());
		}
		out->setData(DataUnit(values,mDim));
	}
	else
	{
		QVector<qint64> values;
		for(BlockInput *i:mArrayInputs)
		{
			DataUnit v=i->data();
			if(v.numType()==DataUnit::S64)
				values.append(((const SensorValueS64*)v.value())->data());
			else
			{
				for(quint32 j=0;j<v.value()->totalCount();++j)
					values.append(v.value()->valueToS64(j));
			}
		}
		out->setData(DataUnit(values,mDim));
	}
}
