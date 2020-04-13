#ifndef LOGICALBLOCKSGROUPXMLPARSERFACTORY_H
#define LOGICALBLOCKSGROUPXMLPARSERFACTORY_H

#include "GDIL/xml/BaseBlocksGroupXmlParserFactory.h"

class LogicalBlocksGroupXmlParserFactory
	:public BaseBlocksGroupXmlParserFactory
{
public:
	LogicalBlocksGroupXmlParserFactory();
	virtual QString groupName()const override;
};

#endif // LOGICALBLOCKSGROUPXMLPARSERFACTORY_H
