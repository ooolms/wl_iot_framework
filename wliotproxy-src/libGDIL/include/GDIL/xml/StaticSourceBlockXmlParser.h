#ifndef STATICSOURCEBLOCKXMLPARSER_H
#define STATICSOURCEBLOCKXMLPARSER_H

#include "GDIL/xml/IBlockXmlParser.h"

class StaticSourceBlockXmlParser
	:public IBlockXmlParser
{
public:
	virtual bool blockFromXml(BaseBlock *block, const QDomElement &blockElem) override;
	virtual void blockToXml(const BaseBlock *block, QDomElement &blockElem) override;
};

#endif // STATICSOURCEBLOCKXMLPARSER_H
