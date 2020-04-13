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

#include "GDIL/xml/BlocksXmlParserFactory.h"
#include "GDIL/xml/DefaultBlockXmlParser.h"
#include "GDIL/blocks/AllBlocks.h"
#include "GDIL/xml/CommandBlockXmlParser.h"
#include "GDIL/xml/ComparationBlockXmlParser.h"
#include "GDIL/xml/DimChangeBlockXmlParser.h"
#include "GDIL/xml/NormingBlockXmlParser.h"
#include "GDIL/xml/StaticSourceBlockXmlParser.h"
#include "GDIL/xml/StorageSourceBlockXmlParser.h"
#include "GDIL/xml/DebugBlockXmlParser.h"

BlocksXmlParserFactory::BlocksXmlParserFactory()
{
	parsersMap[AverageCalcBlock::mTypeId]=new DefaultBlockXmlParser;
	parsersMap[CommandBlock::mTypeId]=new CommandBlockXmlParser;
	parsersMap[ComparationBlock::mTypeId]=new ComparationBlockXmlParser;
	parsersMap[DimChangeBlock::mTypeId]=new DimChangeBlockXmlParser;
	parsersMap[ConditionTransactionBlock::mTypeId]=new DefaultBlockXmlParser;
	parsersMap[NotBoolBlock::mTypeId]=new DefaultBlockXmlParser;
	parsersMap[AndBoolBlock::mTypeId]=new DefaultBlockXmlParser;
	parsersMap[OrBoolBlock::mTypeId]=new DefaultBlockXmlParser;
	parsersMap[XorBoolBlock::mTypeId]=new DefaultBlockXmlParser;
	parsersMap[AndNotBoolBlock::mTypeId]=new DefaultBlockXmlParser;
	parsersMap[OrNotBoolBlock::mTypeId]=new DefaultBlockXmlParser;
	parsersMap[XorNotBoolBlock::mTypeId]=new DefaultBlockXmlParser;
	parsersMap[NormingBlock::mTypeId]=new NormingBlockXmlParser;
	parsersMap[StaticSourceBlock::mTypeId]=new StaticSourceBlockXmlParser;
	parsersMap[StorageSourceBlock::mTypeId]=new StorageSourceBlockXmlParser;
	parsersMap[DebugBlock::mTypeId]=new DebugBlockXmlParser;
}

IBlockXmlParser* BlocksXmlParserFactory::parser(const QUuid &typeId)
{
	return parsersMap.value(typeId);
}
