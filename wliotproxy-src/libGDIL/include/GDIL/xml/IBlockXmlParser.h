#ifndef IBLOCKXMLPARSER_H
#define IBLOCKXMLPARSER_H

#include <QDomElement>
#include "GDIL/core/BaseBlock.h"

class IBlockXmlParser
{
public:
	virtual ~IBlockXmlParser(){}
	virtual bool blockFromXml(BaseBlock *block,const QDomElement &blockElem)=0;
	virtual void blockToXml(const BaseBlock *block,QDomElement &blockElem)=0;
};

#endif // IBLOCKXMLPARSER_H
