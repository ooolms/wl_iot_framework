#ifndef IBLOCKSGROUPXMLPARSERFACTORY_H
#define IBLOCKSGROUPXMLPARSERFACTORY_H

#include "GDIL/xml/IBlockXmlParser.h"

class IBlocksGroupXmlParserFactory
{
public:
	virtual ~IBlocksGroupXmlParserFactory(){}
	virtual QString groupName()const=0;
	virtual QStringList allBlocks()const=0;
	virtual IBlockXmlParser* blockXmlParser(const QString &blockName)=0;
};

#endif // IBLOCKSGROUPXMLPARSERFACTORY_H
