#include "GDIL/xml/LogicalBlocksGroupXmlParserFactory.h"
#include "GDIL/core/LogicalBlocksGroupFactory.h"
#include "GDIL/blocks/LogicalBlocks.h"
#include "GDIL/xml/DefaultBlockXmlParser.h"

LogicalBlocksGroupXmlParserFactory::LogicalBlocksGroupXmlParserFactory()
{
	addParser(NotBoolBlock::mBlockName,new DefaultBlockXmlParser);
	addParser(AndBoolBlock::mBlockName,new DefaultBlockXmlParser);
	addParser(OrBoolBlock::mBlockName,new DefaultBlockXmlParser);
	addParser(XorBoolBlock::mBlockName,new DefaultBlockXmlParser);
	addParser(AndNotBoolBlock::mBlockName,new DefaultBlockXmlParser);
	addParser(OrNotBoolBlock::mBlockName,new DefaultBlockXmlParser);
	addParser(XorNotBoolBlock::mBlockName,new DefaultBlockXmlParser);
}

QString LogicalBlocksGroupXmlParserFactory::groupName()const
{
	return LogicalBlocksGroupFactory::mGroupName;
}
