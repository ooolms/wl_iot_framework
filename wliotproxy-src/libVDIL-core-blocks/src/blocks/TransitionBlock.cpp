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

#include "VDIL/blocks/TransitionBlock.h"
#include "VDIL/core/CoreBlocksGroupFactory.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

const QString TransitionBlock::mBlockName=QString("transition");

TransitionBlock::TransitionBlock(quint32 bId)
	:BaseBlock(bId)
{
	in=mkInput(TypeConstraints(DataUnit::ANY,0),DataUnit::SINGLE,"");
	mCount=0;
}

QString TransitionBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString TransitionBlock::blockName()const
{
	return mBlockName;
}

void TransitionBlock::setCount(quint32 outCount)
{
	mCount=outCount;
	while(outs.count()>mCount)
		rmOutput(outs.takeLast());
	while(outs.count()<mCount)
		outs.append(mkOutput(in->type(),""));
}

quint32 TransitionBlock::count()const
{
	return mCount;
}

void TransitionBlock::eval()
{
	for(BlockOutput *o:outs)
		o->setData(in->data());
}

void TransitionBlock::onInputTypeSelected(BlockInput *)
{
	for(BlockOutput *o:outs)
		o->replaceTypeAndDim(in->type());
}
