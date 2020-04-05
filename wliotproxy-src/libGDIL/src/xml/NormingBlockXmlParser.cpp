#include "GDIL/xml/NormingBlockXmlParser.h"
#include "GDIL/blocks/NormingBlock.h"
#include "GDIL/xml/DataUnitXmlParser.h"

/*
	DataUnit mMinX,mMaxX,mMinY,mMaxY;
	quint32 mDimIndex;
	bool mForceLimits;
*/

bool NormingBlockXmlParser::blockFromXml(BaseBlock *block,const QDomElement &blockElem)
{
	NormingBlock *b=(NormingBlock*)block;
	if(!blockElem.hasAttribute("dim_index")||!blockElem.hasAttribute("force_limits")||!blockElem.hasAttribute("min_x")||
		!blockElem.hasAttribute("max_x")||!blockElem.hasAttribute("min_y")||!blockElem.hasAttribute("max_y"))
			return false;
	bool forceLimits=blockElem.attribute("force_limits")=="1";
	quint32 dimIndex=blockElem.attribute("dim_index").toUInt();
	qint64 minX,maxX,minY,maxY;
	bool ok=false,allS64=true;
	minX=blockElem.attribute("min_x").toLongLong(&ok);
	allS64=allS64&&ok;
	maxX=blockElem.attribute("max_x").toLongLong(&ok);
	allS64=allS64&&ok;
	minY=blockElem.attribute("min_y").toLongLong(&ok);
	allS64=allS64&&ok;
	maxY=blockElem.attribute("max_y").toLongLong(&ok);
	allS64=allS64&&ok;
	if(allS64)
		b->setParams(minX,maxX,minY,maxY,dimIndex,forceLimits);
	else b->setParams(blockElem.attribute("min_x").toDouble(),blockElem.attribute("max_x").toDouble(),
		blockElem.attribute("min_y").toDouble(),blockElem.attribute("max_y").toDouble(),dimIndex,forceLimits);
	return true;
}

void NormingBlockXmlParser::blockToXml(const BaseBlock *block,QDomElement &blockElem)
{
	const NormingBlock *b=(const NormingBlock*)block;
	blockElem.setAttribute("force_limits",b->forceLimits()?"1":"0");
	blockElem.setAttribute("dim_index",b->dimIndex());
	blockElem.setAttribute("min_x",QString::fromUtf8(b->minX().value()->valueToString(0)));
	blockElem.setAttribute("max_x",QString::fromUtf8(b->maxX().value()->valueToString(0)));
	blockElem.setAttribute("min_y",QString::fromUtf8(b->minY().value()->valueToString(0)));
	blockElem.setAttribute("max_y",QString::fromUtf8(b->maxY().value()->valueToString(0)));
}
