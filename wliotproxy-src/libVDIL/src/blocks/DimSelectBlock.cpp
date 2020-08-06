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

#include "VDIL/blocks/DimSelectBlock.h"
#include "VDIL/core/CoreBlocksGroupFactory.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

const QString DimSelectBlock::mBlockName=QString("dim_select");

template<class T>
static void copy1Dim(const SensorValueNumeric<T> *from,SensorValueNumeric<T> *to,quint32 dim)
{
	QVector<T> vals;
	vals.resize((int)from->packetsCount());
	for(quint32 i=0;i<from->packetsCount();++i)
		vals[i]=from->get(dim,i);
	to->setData(vals);
}

DimSelectBlock::DimSelectBlock(quint32 bId)
	:BaseBlock(bId)
{
	mDim=0;
	in=mkInput(TypeConstraints(DataUnit::SINGLE|DataUnit::ARRAY,0),DataUnit::SINGLE,"in");
	out=mkOutput(TypeAndDim(DataUnit::SINGLE,1),"out");
//	hint=QString::fromUtf8("select dim "+QByteArray::number(mDim));
}

QString DimSelectBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString DimSelectBlock::blockName()const
{
	return mBlockName;
}

void DimSelectBlock::setDim(quint32 d)
{
	mDim=d;
}

quint32 DimSelectBlock::dim() const
{
	return mDim;
}

void DimSelectBlock::eval()
{
	if(in->data().dim()>=mDim)
		return;
	SensorDef::Type t=in->data().value()->type();
	t.dim=1;
	QScopedPointer<SensorValue> v(SensorValue::createSensorValue(t));
	if(in->data().numType()==DataUnit::S64)
		copy1Dim((const SensorValueS64*)in->data().value(),(SensorValueS64*)v.data(),mDim);
	else if(in->data().numType()==DataUnit::F64)
		copy1Dim((const SensorValueF64*)in->data().value(),(SensorValueF64*)v.data(),mDim);
	else return;
	out->setData(DataUnit(v.data()));
}

void DimSelectBlock::onInputTypeSelected(BlockInput *b)
{
	Q_UNUSED(b)
	out->replaceTypeAndDim(in->type());
}
