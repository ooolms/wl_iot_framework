#include "GDIL/xml/StaticSourceBlockXmlParser.h"
#include "GDIL/blocks/StaticSourceBlock.h"
#include "GDIL/xml/DataUnitXmlParser.h"

bool StaticSourceBlockXmlParser::blockFromXml(BaseBlock *block,const QDomElement &blockElem)
{
	StaticSourceBlock *b=(StaticSourceBlock*)block;
	QDomElement valueElem=blockElem.firstChildElement("value");
	if(valueElem.isNull())
		return false;
	DataUnit u;
	DataUnitXmlParser::fromXml(u,valueElem);
	b->setValue(u);
	return true;
}

void StaticSourceBlockXmlParser::blockToXml(const BaseBlock *block,QDomElement &blockElem)
{
	QDomElement valueElem=blockElem.ownerDocument().createElement("value");
	blockElem.appendChild(valueElem);
	DataUnitXmlParser::toXml(((const StaticSourceBlock*)block)->value(),valueElem);
}
