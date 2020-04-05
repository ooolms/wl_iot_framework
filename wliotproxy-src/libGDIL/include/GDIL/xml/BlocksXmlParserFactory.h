#ifndef BLOCKSXMLPARSERFACTORY_H
#define BLOCKSXMLPARSERFACTORY_H

#include "GDIL/xml/IBlockXmlParser.h"

class BlocksXmlParserFactory
{
public:
	BlocksXmlParserFactory();
	IBlockXmlParser* parser(const QUuid &typeId);

private:
	QMap<QUuid,IBlockXmlParser*> parsersMap;
};

#endif // BLOCKSXMLPARSERFACTORY_H
