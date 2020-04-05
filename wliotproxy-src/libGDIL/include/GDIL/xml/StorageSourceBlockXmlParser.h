#ifndef STORAGESOURCEBLOCKXMLPARSER_H
#define STORAGESOURCEBLOCKXMLPARSER_H

#include "GDIL/xml/IBlockXmlParser.h"

class StorageSourceBlockXmlParser
	:public IBlockXmlParser
{
public:
	virtual bool blockFromXml(BaseBlock *block, const QDomElement &blockElem) override;
	virtual void blockToXml(const BaseBlock *block, QDomElement &blockElem) override;
};

#endif // STORAGESOURCEBLOCKXMLPARSER_H
