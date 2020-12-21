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

#include "VDIL/xml/ArraySelectBlockXmlParser.h"
#include "VDIL/blocks/ArraySelectBlock.h"

using namespace WLIOTVDIL;

bool ArraySelectBlockXmlParser::blockFromXml(BaseBlock *block,const QDomElement &blockElem,bool tryFixErrors)
{
	ArraySelectBlock *b=(ArraySelectBlock*)block;
	if(!blockElem.hasAttribute("from")||!blockElem.hasAttribute("count"))
		if(!tryFixErrors)return false;
	bool ok=false;
	quint32 from=blockElem.attribute("from").toUInt(&ok);
	if(!ok)return false;
	quint32 count=blockElem.attribute("count").toUInt(&ok);
	if(!ok)return false;
	b->setParams(from,count);
	return true;
}

void ArraySelectBlockXmlParser::blockToXml(const BaseBlock *block,QDomElement &blockElem)
{
	const ArraySelectBlock *b=(const ArraySelectBlock*)block;
	blockElem.setAttribute("from",b->from());
	blockElem.setAttribute("count",b->count());
}
