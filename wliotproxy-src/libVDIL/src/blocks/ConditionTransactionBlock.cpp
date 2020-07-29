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

#include "VDIL/blocks/ConditionTransactionBlock.h"
#include "VDIL/core/CoreBlocksGroupFactory.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

const QString ConditionTransactionBlock::mBlockName=QString("conditional_transaction");

ConditionTransactionBlock::ConditionTransactionBlock(quint32 bId)
	:BaseBlock(bId)
{
	in=mkInput(TypeConstraints(DataUnit::ANY,0),DataUnit::SINGLE,"in");
	boolIn=mkInput(TypeConstraints(DataUnit::BOOL,1),DataUnit::BOOL,"condition");
	out=mkOutput(TypeAndDim(DataUnit::SINGLE,1),"out");
}

QString ConditionTransactionBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString ConditionTransactionBlock::blockName()const
{
	return mBlockName;
}

void ConditionTransactionBlock::eval()
{
	if(boolIn->data().boolValue())
		out->setData(in->data());
}

void ConditionTransactionBlock::onInputTypeSelected(BlockInput *b)
{
	if(b!=in)return;
	out->replaceTypeAndDim(in->type());
}
