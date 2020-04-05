#ifndef COMPARATIONBLOCKXMLPARSER_H
#define COMPARATIONBLOCKXMLPARSER_H

#include "GDIL/xml/IBlockXmlParser.h"

class ComparationBlockXmlParser
	:public IBlockXmlParser
{
public:
	virtual bool blockFromXml(BaseBlock *block, const QDomElement &blockElem)override;
	virtual void blockToXml(const BaseBlock *block, QDomElement &blockElem)override;
};

#endif // COMPARATIONBLOCKXMLPARSER_H
