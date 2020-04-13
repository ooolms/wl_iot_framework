#include "GDIL/xml/CoreBlocksGroupXmlParserFactory.h"
#include "GDIL/core/CoreBlocksGroupFactory.h"
#include "GDIL/xml/DefaultBlockXmlParser.h"
#include "GDIL/blocks/AllBlocks.h"
#include "GDIL/xml/CommandBlockXmlParser.h"
#include "GDIL/xml/ComparationBlockXmlParser.h"
#include "GDIL/xml/DimChangeBlockXmlParser.h"
#include "GDIL/xml/NormingBlockXmlParser.h"
#include "GDIL/xml/StaticSourceBlockXmlParser.h"
#include "GDIL/xml/StorageSourceBlockXmlParser.h"
#include "GDIL/xml/DebugBlockXmlParser.h"

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
}

QString CoreBlocksGroupXmlParserFactory::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}
