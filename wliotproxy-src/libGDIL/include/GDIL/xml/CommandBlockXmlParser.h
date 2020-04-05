#ifndef COMMANDBLOCKXMLPARSER_H
#define COMMANDBLOCKXMLPARSER_H

#include "GDIL/xml/IBlockXmlParser.h"

class CommandBlockXmlParser
	:public IBlockXmlParser
{
public:
	virtual bool blockFromXml(BaseBlock *block,const QDomElement &blockElem)override;
	virtual void blockToXml(const BaseBlock *block,QDomElement &blockElem)override;
};

#endif // COMMANDBLOCKXMLPARSER_H
