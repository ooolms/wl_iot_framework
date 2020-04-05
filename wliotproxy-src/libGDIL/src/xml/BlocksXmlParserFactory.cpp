#include "GDIL/xml/BlocksXmlParserFactory.h"
#include "GDIL/xml/DefaultBlockXmlParser.h"
#include "GDIL/blocks/AllBlocks.h"
#include "GDIL/xml/CommandBlockXmlParser.h"
#include "GDIL/xml/ComparationBlockXmlParser.h"
#include "GDIL/xml/DimChangeBlockXmlParser.h"
#include "GDIL/xml/NormingBlockXmlParser.h"
#include "GDIL/xml/StaticSourceBlockXmlParser.h"
#include "GDIL/xml/StorageSourceBlockXmlParser.h"

BlocksXmlParserFactory::BlocksXmlParserFactory()
{
	parsersMap[AverageCalcBlock::mTypeId]=new DefaultBlockXmlParser;
	parsersMap[CommandBlock::mTypeId]=new CommandBlockXmlParser;
	parsersMap[ComparationBlock::mTypeId]=new ComparationBlockXmlParser;
	parsersMap[DimChangeBlock::mTypeId]=new DimChangeBlockXmlParser;
	parsersMap[EqTransactionBlock::mTypeId]=new DefaultBlockXmlParser;
	parsersMap[NotBoolBlock::mTypeId]=new DefaultBlockXmlParser;
	parsersMap[AndBoolBlock::mTypeId]=new DefaultBlockXmlParser;
	parsersMap[OrBoolBlock::mTypeId]=new DefaultBlockXmlParser;
	parsersMap[XorBoolBlock::mTypeId]=new DefaultBlockXmlParser;
	parsersMap[AndNotBoolBlock::mTypeId]=new DefaultBlockXmlParser;
	parsersMap[OrNotBoolBlock::mTypeId]=new DefaultBlockXmlParser;
	parsersMap[NormingBlock::mTypeId]=new NormingBlockXmlParser;
	parsersMap[StaticSourceBlock::mTypeId]=new StaticSourceBlockXmlParser;
	parsersMap[StorageSourceBlock::mTypeId]=new StorageSourceBlockXmlParser;
}

IBlockXmlParser* BlocksXmlParserFactory::parser(const QUuid &typeId)
{
	return parsersMap.value(typeId);
}
