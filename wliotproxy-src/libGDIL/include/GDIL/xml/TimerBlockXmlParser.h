#ifndef TIMERBLOCKXMLPARSER_H
#define TIMERBLOCKXMLPARSER_H

#include "GDIL/xml/IBlockXmlParser.h"

class TimerBlockXmlParser
	:public IBlockXmlParser
{
public:
	virtual bool blockFromXml(BaseBlock *block,const QDomElement &blockElem)override;
	virtual void blockToXml(const BaseBlock *block,QDomElement &blockElem)override;
};

#endif // TIMERBLOCKXMLPARSER_H
