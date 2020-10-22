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

#include "VDIL/core/CoreBlocksGroupFactory.h"
#include "VDIL/blocks/AverageCalcBlock.h"
#include "VDIL/blocks/CommandBlock.h"
#include "VDIL/blocks/ComparationBlock.h"
#include "VDIL/blocks/DimSelectBlock.h"
#include "VDIL/blocks/ConditionTransitionBlock.h"
#include "VDIL/blocks/LinearTransformationBlock.h"
#include "VDIL/blocks/StaticSourceBlock.h"
#include "VDIL/blocks/StorageSourceBlock.h"
#include "VDIL/blocks/DebugBlock.h"
#include "VDIL/blocks/DelayBlock.h"
#include "VDIL/blocks/DeviceStateSourceBlock.h"
#include "VDIL/blocks/DateTimeSourceBlock.h"
#include "VDIL/blocks/ArrayCombineBlock.h"
#include "VDIL/blocks/ArraySelectBlock.h"
#include "VDIL/blocks/RandomSourceBlock.h"
#include "VDIL/core/RuntimeSourceBlock.h"
#include "VDIL/core/RuntimeStoreBlock.h"
#include "VDIL/core/TimerBlock.h"
#include "VDIL/blocks/DevicePresenceSourceBlock.h"
#include "VDIL/core/VDevCommandSourceBlock.h"
#include "VDIL/core/VDevSensorSendBlock.h"
#include "VDIL/blocks/MathExpBlock.h"
#include "VDIL/core/SubProgramBlock.h"
#include "VDIL/core/SubProgram.h"
#include "VDIL/blocks/TransitionBlock.h"
#include "VDIL/core/Program.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

const QString CoreBlocksGroupFactory::mGroupName=QString("core");

QString CoreBlocksGroupFactory::groupName()const
{
	return Program::reservedCoreGroupName;
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
	else if(name==DimSelectBlock::mBlockName)
		return new DimSelectBlock(blockId);
	else if(name==ConditionTransitionBlock::mBlockName)
		return new ConditionTransitionBlock(blockId);
	else if(name==LinearTransformationBlock::mBlockName)
		return new LinearTransformationBlock(blockId);
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
	else if(name==DateTimeSourceBlock::mBlockName)
		return new DateTimeSourceBlock(blockId);
	else if(name==ArrayCombineBlock::mBlockName)
		return new ArrayCombineBlock(blockId);
	else if(name==ArraySelectBlock::mBlockName)
		return new ArraySelectBlock(blockId);
	else if(name==RandomSourceBlock::mBlockName)
		return new RandomSourceBlock(blockId);
	else if(name==DevicePresenceSourceBlock::mBlockName)
		return new DevicePresenceSourceBlock(blockId);
	else if(name==RuntimeSourceBlock::mBlockName)
		return new RuntimeSourceBlock(blockId);
	else if(name==RuntimeStoreBlock::mBlockName)
		return new RuntimeStoreBlock(blockId);
	else if(name==VDevSensorSendBlock::mBlockName)
		return new VDevSensorSendBlock(blockId);
	else if(name==VDevCommandSourceBlock::mBlockName)
		return new VDevCommandSourceBlock(blockId);
	else if(name==MathExpBlock::mBlockName)
		return new MathExpBlock(blockId);
	else if(name==SubProgramBlock::mBlockName)
		return new SubProgramBlock(blockId);
	else if(name==TransitionBlock::mBlockName)
		return new TransitionBlock(blockId);
	else if(name==SubProgramInternalInputBlock::mBlockName)
		return new SubProgramInternalInputBlock(blockId);
	else if(name==SubProgramInternalOutputBlock::mBlockName)
		return new SubProgramInternalOutputBlock(blockId);
	else return 0;
}
