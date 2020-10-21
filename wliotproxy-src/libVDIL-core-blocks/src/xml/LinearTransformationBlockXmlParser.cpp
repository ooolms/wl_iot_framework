/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "VDIL/xml/LinearTransformationBlockXmlParser.h"
#include "VDIL/blocks/LinearTransformationBlock.h"
#include "VDIL/xml/DataUnitXmlParser.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

bool LinearTransformationBlockXmlParser::blockFromXml(BaseBlock *block,const QDomElement &blockElem,bool tryFixErrors)
{
	LinearTransformationBlock *b=(LinearTransformationBlock*)block;
	if(!blockElem.hasAttribute("dim_index")||!blockElem.hasAttribute("force_limits")||!blockElem.hasAttribute("min_x")||
		!blockElem.hasAttribute("max_x")||!blockElem.hasAttribute("min_y")||!blockElem.hasAttribute("max_y"))
		if(!tryFixErrors)return false;
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
	else
	{
		double minX,maxX,minY,maxY;
		minX=blockElem.attribute("min_x").toDouble(&ok);
		if(!ok)return false;
		maxX=blockElem.attribute("max_x").toDouble(&ok);
		if(!ok)return false;
		minY=blockElem.attribute("min_y").toDouble(&ok);
		if(!ok)return false;
		maxY=blockElem.attribute("max_y").toDouble(&ok);
		if(!ok)return false;
		b->setParams(minX,maxX,minY,maxY,dimIndex,forceLimits);
	}
	return true;
}

void LinearTransformationBlockXmlParser::blockToXml(const BaseBlock *block,QDomElement &blockElem)
{
	const LinearTransformationBlock *b=(const LinearTransformationBlock*)block;
	blockElem.setAttribute("force_limits",QString::fromUtf8(b->forceLimits()?"1":"0"));
	blockElem.setAttribute("dim_index",b->dimIndex());
	blockElem.setAttribute("min_x",QString::fromUtf8(b->minX().value()->valueToString(0)));
	blockElem.setAttribute("max_x",QString::fromUtf8(b->maxX().value()->valueToString(0)));
	blockElem.setAttribute("min_y",QString::fromUtf8(b->minY().value()->valueToString(0)));
	blockElem.setAttribute("max_y",QString::fromUtf8(b->maxY().value()->valueToString(0)));
}
