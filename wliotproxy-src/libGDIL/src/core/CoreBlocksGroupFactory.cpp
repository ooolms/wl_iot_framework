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

#include "GDIL/core/CoreBlocksGroupFactory.h"
#include "GDIL/blocks/AverageCalcBlock.h"
#include "GDIL/blocks/CommandBlock.h"
#include "GDIL/blocks/ComparationBlock.h"
#include "GDIL/blocks/DimChangeBlock.h"
#include "GDIL/blocks/ConditionTransactionBlock.h"
#include "GDIL/blocks/NormingBlock.h"
#include "GDIL/blocks/StaticSourceBlock.h"
#include "GDIL/blocks/StorageSourceBlock.h"
#include "GDIL/blocks/DebugBlock.h"
#include "GDIL/blocks/DelayBlock.h"
#include "GDIL/blocks/DeviceStateSourceBlock.h"
#include "GDIL/core/TimerBlock.h"

using namespace WLIOT;
using namespace WLIOTGDIL;

const QString CoreBlocksGroupFactory::mGroupName=QString("core");

QString CoreBlocksGroupFactory::groupName()const
{
	return mGroupName;
}

QStringList CoreBlocksGroupFactory::allBlocks()const
{
	return QStringList();
}

BaseBlock* CoreBlocksGroupFactory::makeBlock(const QString &name,quint32 blockId)
{
	if(name==AverageCalcBlock::mBlockName)
		return new AverageCalcBlock(blockId);
	else if(name==CommandBlock::mBlockName)
		return new CommandBlock(blockId);
	else if(name==ComparationBlock::mBlockName)
		return new ComparationBlock(blockId);
	else if(name==DimChangeBlock::mBlockName)
		return new DimChangeBlock(blockId);
	else if(name==ConditionTransactionBlock::mBlockName)
		return new ConditionTransactionBlock(blockId);
	else if(name==NormingBlock::mBlockName)
		return new NormingBlock(blockId);
	else if(name==StaticSourceBlock::mBlockName)
		return new StaticSourceBlock(blockId);
	else if(name==StorageSourceBlock::mBlockName)
		return new StorageSourceBlock(blockId);
	else if(name==DebugBlock::mBlockName)
		return new DebugBlock(blockId);
	else if(name==DelayBlock::mBlockName)
		return new DelayBlock(blockId);
	else if(name==DeviceStateSourceBlock::mBlockName)
		return new DeviceStateSourceBlock(blockId);
	else if(name==TimerBlock::mBlockName)
		return new TimerBlock(blockId);
	else return 0;
}
