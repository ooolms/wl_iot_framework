#include "GDIL/xml/ComparationBlockXmlParser.h"
#include "GDIL/blocks/ComparationBlock.h"
#include "GDIL/xml/DataUnitXmlParser.h"

static QString opToStr(ComparationBlock::Operation op)
{
	if(op==ComparationBlock::EQ)
		return "==";
	else if(op==ComparationBlock::NEQ)
		return "!=";
	else if(op==ComparationBlock::LT)
		return "<";
	else if(op==ComparationBlock::GT)
		return ">";
	else if(op==ComparationBlock::LTEQ)
		return "<=";
	else if(op==ComparationBlock::GTEQ)
		return ">=";
	else if(op==ComparationBlock::DISTANCE)
		return "dist";
	else return "";
}

static ComparationBlock::Operation opFromStr(const QString &s)
{
	if(s=="!=")
		return ComparationBlock::NEQ;
	else if(s=="<")
		return ComparationBlock::LT;
	else if(s==">")
		return ComparationBlock::GT;
	else if(s=="<=")
		return ComparationBlock::LTEQ;
	else if(s==">=")
		return ComparationBlock::GTEQ;
	else if(s=="dist")
		return ComparationBlock::DISTANCE;
	else return ComparationBlock::EQ;
}

bool ComparationBlockXmlParser::blockFromXml(BaseBlock *block,const QDomElement &blockElem)
{
	ComparationBlock *b=(ComparationBlock*)block;
	QDomElement distValElem=blockElem.firstChildElement("dist_value");
	if(!blockElem.hasAttribute("bool_out_mode")||!blockElem.hasAttribute("operation")||
		!blockElem.hasAttribute("dim_index")||distValElem.isNull())
			return false;
	bool boolOutMode=blockElem.attribute("bool_out_mode")=="1";
	ComparationBlock::Operation op=opFromStr(blockElem.attribute("operation"));
	quint32 dimIndex=blockElem.attribute("dim_index").toUInt();
	DataUnit distValue;
	DataUnitXmlParser::fromXml(distValue,distValElem);
	if(!distValue.isValid())
		distValue=DataUnit(1.0);
	b->setParams(boolOutMode,dimIndex,op);
	b->setDistValue(distValue);
	return true;
}

void ComparationBlockXmlParser::blockToXml(const BaseBlock *block,QDomElement &blockElem)
{
	const ComparationBlock *b=(const ComparationBlock*)block;
	blockElem.setAttribute("bool_out_mode",b->boolOutMode()?"1":"0");
	blockElem.setAttribute("operation",opToStr(b->operation()));
	blockElem.setAttribute("dim_index",b->dimIndex());
	QDomElement distValElem=blockElem.ownerDocument().createElement("dist_value");
	blockElem.appendChild(distValElem);
	DataUnitXmlParser::toXml(b->distValue(),distValElem);
}
