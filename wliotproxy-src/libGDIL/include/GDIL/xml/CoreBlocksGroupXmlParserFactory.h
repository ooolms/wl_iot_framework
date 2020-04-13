#ifndef COREBLOCKSGROUPXMLPARSERFACTORY_H
#define COREBLOCKSGROUPXMLPARSERFACTORY_H

#include "GDIL/xml/BaseBlocksGroupXmlParserFactory.h"

class CoreBlocksGroupXmlParserFactory
	:public BaseBlocksGroupXmlParserFactory
{
public:
	CoreBlocksGroupXmlParserFactory();
	virtual QString groupName()const override;
};

#endif // COREBLOCKSGROUPXMLPARSERFACTORY_H
