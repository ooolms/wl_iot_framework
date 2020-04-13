#include "GDIL/xml/BaseBlocksGroupXmlParserFactory.h"

BaseBlocksGroupXmlParserFactory::~BaseBlocksGroupXmlParserFactory()
{
	for(auto v:mParsers)
		delete v;
}

QStringList BaseBlocksGroupXmlParserFactory::allBlocks()const
{
	return mParsers.keys();
}

IBlockXmlParser* BaseBlocksGroupXmlParserFactory::blockXmlParser(const QString &blockName)
{
	return mParsers.value(blockName);
}

void BaseBlocksGroupXmlParserFactory::addParser(const QString &blockName,IBlockXmlParser *parser)
{
	mParsers[blockName]=parser;
}
