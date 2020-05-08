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

#include "GDIL/xml/DebugBlockXmlParser.h"
#include "GDIL/blocks/DebugBlock.h"

bool DebugBlockXmlParser::blockFromXml(BaseBlock *block,const QDomElement &blockElem)
{
	if(!blockElem.hasAttribute("debug_string")||!blockElem.hasAttribute("inputs_count"))
		return false;
	quint32 inputsCount=blockElem.attribute("inputs_count").toUInt();
	((DebugBlock*)block)->setParams(blockElem.attribute("debug_string"),inputsCount);
	return true;
}

void DebugBlockXmlParser::blockToXml(const BaseBlock *block,QDomElement &blockElem)
{
	const DebugBlock *b=(const DebugBlock*)block;
	blockElem.setAttribute("debug_string",b->debugString());
	blockElem.setAttribute("inputs_count",b->inCount());
}