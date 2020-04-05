#ifndef NORMINGBLOCKXMLPARSER_H
#define NORMINGBLOCKXMLPARSER_H

#include "GDIL/xml/IBlockXmlParser.h"

class NormingBlockXmlParser
	:public IBlockXmlParser
{
public:
	virtual bool blockFromXml(BaseBlock *block, const QDomElement &blockElem) override;
	virtual void blockToXml(const BaseBlock *block, QDomElement &blockElem) override;
};

#endif // NORMINGBLOCKXMLPARSER_H
