#ifndef DIMCHANGEBLOCKXMLPARSER_H
#define DIMCHANGEBLOCKXMLPARSER_H

#include "GDIL/xml/IBlockXmlParser.h"

class DimChangeBlockXmlParser
	:public IBlockXmlParser
{
public:
	virtual bool blockFromXml(BaseBlock *block,const QDomElement &blockElem)override;
	virtual void blockToXml(const BaseBlock *block,QDomElement &blockElem)override;
};

#endif // DIMCHANGEBLOCKXMLPARSER_H
