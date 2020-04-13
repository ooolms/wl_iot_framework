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

#include "GDIL/blocks/ConditionTransactionBlock.h"

const QUuid ConditionTransactionBlock::mTypeId=QUuid("{2faf94c6-9b09-4edc-a97b-8680e59a5500}");

ConditionTransactionBlock::ConditionTransactionBlock(quint32 bId)
	:BaseBlock(bId)
{
	in=mkInput(DataUnit::ANY,DataUnit::SINGLE,0,"in");
	boolIn=mkInput(DataUnit::BOOL,DataUnit::BOOL,1,"condition");
	out=mkOutput(DataUnit::SINGLE,1,"out");
}

QUuid ConditionTransactionBlock::typeId()const
{
	return mTypeId;
}

void ConditionTransactionBlock::eval()
{
	const SensorValueU8 *bVal=(const SensorValueU8*)boolIn->data().value();
	if(bVal->getValue(0)==1)
		out->setData(in->data());
}

void ConditionTransactionBlock::onInputTypeSelected(BlockInput *b)
{
	if(b!=in)return;
	out->replaceTypeAndDim(in->type(),in->dim());
}
