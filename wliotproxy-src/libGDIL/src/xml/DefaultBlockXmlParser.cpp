#include "GDIL/xml/DefaultBlockXmlParser.h"

bool DefaultBlockXmlParser::blockFromXml(BaseBlock *,const QDomElement &)
{
	return true;
}

void DefaultBlockXmlParser::blockToXml(const BaseBlock *,QDomElement &)
{
}
