#ifndef DEFAULTBLOCKXMLPARSER_H
#define DEFAULTBLOCKXMLPARSER_H

#include "GDIL/xml/IBlockXmlParser.h"

class DefaultBlockXmlParser
	:public IBlockXmlParser
{
public:
	virtual bool blockFromXml(BaseBlock *block,const QDomElement &blockElem)override;
	virtual void blockToXml(const BaseBlock *block,QDomElement &blockElem)override;
};

#endif // DEFAULTBLOCKXMLPARSER_H
