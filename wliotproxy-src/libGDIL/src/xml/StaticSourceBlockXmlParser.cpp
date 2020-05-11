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

#include "GDIL/xml/StaticSourceBlockXmlParser.h"
#include "GDIL/blocks/StaticSourceBlock.h"
#include "GDIL/xml/DataUnitXmlParser.h"

bool StaticSourceBlockXmlParser::blockFromXml(BaseBlock *block,const QDomElement &blockElem)
{
	StaticSourceBlock *b=(StaticSourceBlock*)block;
	QDomElement valueElem=blockElem.firstChildElement("value");
	if(valueElem.isNull()||!blockElem.hasAttribute("configurable"))
		return false;
	DataUnit u;
	DataUnitXmlParser::fromXml(u,valueElem);
	b->setParams(u,blockElem.attribute("configurable")=="1");
	return true;
}

void StaticSourceBlockXmlParser::blockToXml(const BaseBlock *block,QDomElement &blockElem)
{
	QDomElement valueElem=blockElem.ownerDocument().createElement("value");
	blockElem.appendChild(valueElem);
	DataUnitXmlParser::toXml(((const StaticSourceBlock*)block)->value(),valueElem);
	blockElem.setAttribute("configurable",QString::fromUtf8(((const StaticSourceBlock*)block)->configurable()?"1":"0"));
}
