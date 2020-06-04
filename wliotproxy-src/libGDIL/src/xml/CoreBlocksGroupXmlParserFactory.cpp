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

#include "GDIL/xml/CoreBlocksGroupXmlParserFactory.h"
#include "GDIL/core/CoreBlocksGroupFactory.h"
#include "GDIL/xml/DefaultBlockXmlParser.h"
#include "GDIL/xml/CommandBlockXmlParser.h"
#include "GDIL/xml/ComparationBlockXmlParser.h"
#include "GDIL/xml/DimChangeBlockXmlParser.h"
#include "GDIL/xml/NormingBlockXmlParser.h"
#include "GDIL/xml/StaticSourceBlockXmlParser.h"
#include "GDIL/xml/StorageSourceBlockXmlParser.h"
#include "GDIL/xml/DebugBlockXmlParser.h"
#include "GDIL/xml/DelayBlockXmlParser.h"
#include "GDIL/xml/TimerBlockXmlParser.h"
#include "GDIL/xml/DeviceStateSourceBlockXmlParser.h"
#include "GDIL/xml/DateTimeSourceBlockXmlParser.h"
#include "GDIL/blocks/AverageCalcBlock.h"
#include "GDIL/blocks/CommandBlock.h"
#include "GDIL/blocks/ComparationBlock.h"
#include "GDIL/blocks/ConditionTransactionBlock.h"
#include "GDIL/blocks/DebugBlock.h"
#include "GDIL/blocks/DimChangeBlock.h"
#include "GDIL/blocks/NormingBlock.h"
#include "GDIL/blocks/StaticSourceBlock.h"
#include "GDIL/blocks/StorageSourceBlock.h"
#include "GDIL/blocks/DelayBlock.h"
#include "GDIL/blocks/DeviceStateSourceBlock.h"
#include "GDIL/blocks/DateTimeSourceBlock.h"
#include "GDIL/core/TimerBlock.h"

using namespace WLIOT;
using namespace WLIOTGDIL;

CoreBlocksGroupXmlParserFactory::CoreBlocksGroupXmlParserFactory()
{
	addParser(AverageCalcBlock::mBlockName,new DefaultBlockXmlParser);
	addParser(CommandBlock::mBlockName,new CommandBlockXmlParser);
	addParser(ComparationBlock::mBlockName,new ComparationBlockXmlParser);
	addParser(DimChangeBlock::mBlockName,new DimChangeBlockXmlParser);
	addParser(ConditionTransactionBlock::mBlockName,new DefaultBlockXmlParser);
	addParser(NormingBlock::mBlockName,new NormingBlockXmlParser);
	addParser(StaticSourceBlock::mBlockName,new StaticSourceBlockXmlParser);
	addParser(StorageSourceBlock::mBlockName,new StorageSourceBlockXmlParser);
	addParser(DebugBlock::mBlockName,new DebugBlockXmlParser);
	addParser(DelayBlock::mBlockName,new DelayBlockXmlParser);
	addParser(TimerBlock::mBlockName,new TimerBlockXmlParser);
	addParser(DeviceStateSourceBlock::mBlockName,new DeviceStateSourceBlockXmlParser);
	addParser(DateTimeSourceBlock::mBlockName,new DateTimeBlockXmlParser);
}

QString CoreBlocksGroupXmlParserFactory::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}
