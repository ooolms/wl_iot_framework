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

#include "GDIL/blocks/AverageCalcBlock.h"
#include "GDIL/core/CoreBlocksGroupFactory.h"

const QString AverageCalcBlock::mBlockName=QString("average_calc");

template<class T>
static void avCalc(const SensorValueNumeric<T> *in,SensorValueNumeric<T> *out)
{
	quint32 count=in->packetsCount();
	quint32 dim=in->type().dim;
	QVector<T> av;
	av.resize(dim);
	for(quint32 i=0;i<count;++i)
		for(quint32 j=0;j<dim;++j)
			av[j]+=in->valueToDouble(j,i);
	for(quint32 i=0;i<dim;++i)
		av[i]/=count;
	out->setData(av);
}

AverageCalcBlock::AverageCalcBlock(quint32 bId)
	:BaseBlock(bId)
{
	in=mkInput(DataUnit::SINGLE|DataUnit::ARRAY,DataUnit::SINGLE,0,"in");
	out=mkOutput(DataUnit::SINGLE,1,"average");
}

QString AverageCalcBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString AverageCalcBlock::blockName()const
{
	return mBlockName;
}

void AverageCalcBlock::eval()
{
	if(in->data().type()==DataUnit::SINGLE)
		out->setData(DataUnit(in->data().value()));
	else if(in->data().type()==DataUnit::ARRAY&&in->data().value()->packetsCount()>0)
	{
		SensorDef::Type t=in->data().value()->type();
		t.packType=SensorDef::SINGLE;
		QScopedPointer<SensorValue> v(SensorValue::createSensorValue(t));
		if(in->data().numType()==DataUnit::S64)
			avCalc((const SensorValueS64*)in->data().value(),(SensorValueS64*)v.data());
		else if(in->data().numType()==DataUnit::F64)
			avCalc((const SensorValueF64*)in->data().value(),(SensorValueF64*)v.data());
		else return;
		out->setData(DataUnit(v.data()));
	}
}

void AverageCalcBlock::onInputTypeSelected(BlockInput *b)
{
	Q_UNUSED(b)
	out->replaceTypeAndDim(DataUnit::SINGLE,in->dim());
}
