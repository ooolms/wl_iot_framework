#ifndef BASEBLOCKSGROUPXMLPARSERFACTORY_H
#define BASEBLOCKSGROUPXMLPARSERFACTORY_H

#include "GDIL/xml/IBlocksGroupXmlParserFactory.h"

class BaseBlocksGroupXmlParserFactory
	:public IBlocksGroupXmlParserFactory
{
public:
	virtual ~BaseBlocksGroupXmlParserFactory();
	virtual QStringList allBlocks()const override;
	virtual IBlockXmlParser *blockXmlParser(const QString &blockName)override;

protected:
	void addParser(const QString &blockName,IBlockXmlParser *parser);

private:
	QMap<QString,IBlockXmlParser*> mParsers;
};

#endif // BASEBLOCKSGROUPXMLPARSERFACTORY_H
