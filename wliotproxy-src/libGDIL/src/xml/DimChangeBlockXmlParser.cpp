#include "GDIL/xml/DimChangeBlockXmlParser.h"
#include "GDIL/blocks/DimChangeBlock.h"

bool DimChangeBlockXmlParser::blockFromXml(BaseBlock *block,const QDomElement &blockElem)
{
	if(!blockElem.hasAttribute("dim"))
		return false;
	((DimChangeBlock*)block)->setDim(blockElem.attribute("dim").toUInt());
	return true;
}

void DimChangeBlockXmlParser::blockToXml(const BaseBlock *block,QDomElement &blockElem)
{
	blockElem.setAttribute("dim",((const DimChangeBlock*)block)->dim());
}
