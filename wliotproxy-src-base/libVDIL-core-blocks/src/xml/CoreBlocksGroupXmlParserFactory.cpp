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

#include "VDIL/xml/CoreBlocksGroupXmlParserFactory.h"
#include "VDIL/core/CoreBlocksGroupFactory.h"
#include "VDIL/xml/DefaultBlockXmlParser.h"
#include "VDIL/xml/CommandBlockXmlParser.h"
#include "VDIL/xml/ComparationBlockXmlParser.h"
#include "VDIL/xml/DimSelectBlockXmlParser.h"
#include "VDIL/xml/LinearTransformationBlockXmlParser.h"
#include "VDIL/xml/StaticSourceBlockXmlParser.h"
#include "VDIL/xml/StorageSourceBlockXmlParser.h"
#include "VDIL/xml/DebugBlockXmlParser.h"
#include "VDIL/xml/DelayBlockXmlParser.h"
#include "VDIL/xml/TimerBlockXmlParser.h"
#include "VDIL/xml/DeviceStateSourceBlockXmlParser.h"
#include "VDIL/xml/DateTimeSourceBlockXmlParser.h"
#include "VDIL/xml/ArrayCombineBlockXmlParser.h"
#include "VDIL/xml/ArraySelectBlockXmlParser.h"
#include "VDIL/xml/RandomSourceBlockXmlParser.h"
#include "VDIL/xml/DevicePresenceSourceBlockXmlParser.h"
#include "VDIL/xml/RuntimeSourceBlockXmlParser.h"
#include "VDIL/xml/RuntimeStoreBlockXmlParser.h"
#include "VDIL/xml/VDevCommandSourceBlockXmlParser.h"
#include "VDIL/xml/VDevSensorSendBlockXmlParser.h"
#include "VDIL/xml/MathExpBlockXmlParser.h"
#include "VDIL/xml/TransitionBlockXmlParser.h"
#include "VDIL/xml/SubProgramInternalBlocksParser.h"
#include "VDIL/xml/VDevAdditionalStateChangeBlockXmlParser.h"

#include "VDIL/blocks/AverageCalcBlock.h"
#include "VDIL/blocks/CommandBlock.h"
#include "VDIL/blocks/ComparationBlock.h"
#include "VDIL/blocks/ConditionTransitionBlock.h"
#include "VDIL/blocks/DebugBlock.h"
#include "VDIL/blocks/DimSelectBlock.h"
#include "VDIL/blocks/LinearTransformationBlock.h"
#include "VDIL/blocks/StaticSourceBlock.h"
#include "VDIL/blocks/StorageSourceBlock.h"
#include "VDIL/blocks/DelayBlock.h"
#include "VDIL/blocks/DeviceStateSourceBlock.h"
#include "VDIL/blocks/DateTimeSourceBlock.h"
#include "VDIL/blocks/ArrayCombineBlock.h"
#include "VDIL/blocks/ArraySelectBlock.h"
#include "VDIL/blocks/RandomSourceBlock.h"
#include "VDIL/core/TimerBlock.h"
#include "VDIL/blocks/DevicePresenceSourceBlock.h"
#include "VDIL/core/RuntimeSourceBlock.h"
#include "VDIL/core/RuntimeStoreBlock.h"
#include "VDIL/core/VDevCommandSourceBlock.h"
#include "VDIL/core/VDevSensorSendBlock.h"
#include "VDIL/blocks/MathExpBlock.h"
#include "VDIL/blocks/TransitionBlock.h"
#include "VDIL/core/SubProgram.h"
#include "VDIL/core/Program.h"
#include "VDIL/core/VDevAdditionalStateChangeBlock.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

CoreBlocksGroupXmlParserFactory::CoreBlocksGroupXmlParserFactory()
{
	addParser(AverageCalcBlock::mBlockName,new DefaultBlockXmlParser);
	addParser(CommandBlock::mBlockName,new CommandBlockXmlParser);
	addParser(ComparationBlock::mBlockName,new ComparationBlockXmlParser);
	addParser(DimSelectBlock::mBlockName,new DimSelectBlockXmlParser);
	addParser(ConditionTransitionBlock::mBlockName,new DefaultBlockXmlParser);
	addParser(LinearTransformationBlock::mBlockName,new LinearTransformationBlockXmlParser);
	addParser(StaticSourceBlock::mBlockName,new StaticSourceBlockXmlParser);
	addParser(StorageSourceBlock::mBlockName,new StorageSourceBlockXmlParser);
	addParser(DebugBlock::mBlockName,new DebugBlockXmlParser);
	addParser(DelayBlock::mBlockName,new DelayBlockXmlParser);
	addParser(TimerBlock::mBlockName,new TimerBlockXmlParser);
	addParser(DeviceStateSourceBlock::mBlockName,new DeviceStateSourceBlockXmlParser);
	addParser(DateTimeSourceBlock::mBlockName,new DateTimeBlockXmlParser);
	addParser(ArrayCombineBlock::mBlockName,new ArrayCombineBlockXmlParser);
	addParser(ArraySelectBlock::mBlockName,new ArraySelectBlockXmlParser);
	addParser(RandomSourceBlock::mBlockName,new RandomSourceBlockXmlParser);
	addParser(DevicePresenceSourceBlock::mBlockName,new DevicePresenceSourceBlockXmlParser);
	addParser(RuntimeSourceBlock::mBlockName,new RuntimeSourceBlockXmlParser);
	addParser(RuntimeStoreBlock::mBlockName,new RuntimeStoreBlockXmlParser);
	addParser(VDevSensorSendBlock::mBlockName,new VDevSensorSendBlockXmlParser);
	addParser(VDevCommandSourceBlock::mBlockName,new VDevCommandSourceBlockXmlParser);
	addParser(MathExpBlock::mBlockName,new MathExpBlockXmlParser);
	addParser(TransitionBlock::mBlockName,new TransitionBlockXmlParser);
	addParser(SubProgramInternalInputBlock::mBlockName,new SubProgramInternalInputBlockXmlParser);
	addParser(SubProgramInternalOutputBlock::mBlockName,new SubProgramInternalOutputBlockXmlParser);
	addParser(VDevAdditionalStateChangeBlock::mBlockName,new VDevAdditionalStateChangeBlockXmlParser);
}

QString CoreBlocksGroupXmlParserFactory::groupName()const
{
	return Program::reservedCoreGroupName;
}
