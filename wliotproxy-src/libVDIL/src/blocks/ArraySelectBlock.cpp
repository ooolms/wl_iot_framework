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

#include "VDIL/blocks/ArraySelectBlock.h"
#include "VDIL/core/CoreBlocksGroupFactory.h"
#include "wliot/devices/SensorValue.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

const QString ArraySelectBlock::mBlockName=QString("array_select");

ArraySelectBlock::ArraySelectBlock(quint32 bId)
	:BaseBlock(bId)
{
	in=mkInput(TypeConstraints(DataUnit::ARRAY,0),DataUnit::SINGLE,"in");
	mFrom=0;
	mCount=1;
	out=mkOutput(DataUnit::SINGLE,1,"out");
}

QString ArraySelectBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString ArraySelectBlock::blockName()const
{
	return mBlockName;
}

void ArraySelectBlock::setParams(quint32 from,quint32 count)
{
	mFrom=from;
	mCount=count;
	if(mCount==0)
		mCount=1;
	DataUnit::Type type=(mCount==1)?DataUnit::SINGLE:DataUnit::ARRAY;
	if(type!=out->type())
		out->replaceTypeAndDim(type,in->dim());
}

quint32 ArraySelectBlock::from()const
{
	return mFrom;
}

quint32 ArraySelectBlock::count()const
{
	return mCount;
}

void ArraySelectBlock::eval()
{
	DataUnit v=in->data();
	if(mFrom>=v.value()->packetsCount())
		return;
	if(v.numType()==DataUnit::S64)
	{
		QVector<qint64> values;
		values.append(((const SensorValueS64*)v.value())->getSamples(mFrom,mCount));
		if(values.isEmpty())return;
		if(mCount==1)
			out->setData(DataUnit(values));
		else out->setData(DataUnit(values,in->dim()));
	}
	else
	{
		QVector<double> values;
		values.append(((const SensorValueF64*)v.value())->getSamples(mFrom,mCount));
		if(values.isEmpty())return;
		if(mCount==1)
			out->setData(DataUnit(values));
		else out->setData(DataUnit(values,in->dim()));
	}
}

void ArraySelectBlock::onInputTypeSelected(BlockInput*)
{
	out->replaceTypeAndDim(DataUnit::SINGLE,in->dim());
}
