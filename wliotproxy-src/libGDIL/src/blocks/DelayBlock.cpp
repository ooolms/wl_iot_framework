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

#include "GDIL/blocks/DelayBlock.h"
#include "GDIL/core/CoreBlocksGroupFactory.h"
#include <QThread>

using namespace WLIOT;
using namespace WLIOTGDIL;

const QString DelayBlock::mBlockName=QString("delay");

DelayBlock::DelayBlock(quint32 blockId)
	:BaseBlock(blockId)
{
	in=mkInput(TypeConstraints(DataUnit::ANY,0),DataUnit::SINGLE,"in");
	out=mkOutput(DataUnit::SINGLE,1,"out");
	mDelay=100;
}

QString DelayBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString DelayBlock::blockName()const
{
	return mBlockName;
}

quint32 DelayBlock::delay()const
{
	return mDelay;
}

void DelayBlock::setDelay(quint32 msec)
{
	mDelay=msec;
}

void DelayBlock::eval()
{
	evalOnTimerInMsec(mDelay);
}

void DelayBlock::evalOnTimer()
{
	out->setData(in->data());
}

void DelayBlock::onInputTypeSelected(BlockInput*)
{
	out->replaceTypeAndDim(in->type(),in->dim());
}
