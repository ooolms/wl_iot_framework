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

#include "GDIL/xml/DateTimeSourceBlockXmlParser.h"
#include "GDIL/blocks/DateTimeSourceBlock.h"

using namespace WLIOTGDIL;


bool DateTimeBlockXmlParser::blockFromXml(BaseBlock *block,const QDomElement &blockElem)
{
	if(!blockElem.hasAttribute("outputs"))
		return false;
	((DateTimeSourceBlock*)block)->setDateOutputs(
		DateTimeSourceBlock::dateOutputsFromStr(blockElem.attribute("outputs")));
	return true;
}

void DateTimeBlockXmlParser::blockToXml(const BaseBlock *block,QDomElement &blockElem)
{
	blockElem.setAttribute("outputs",DateTimeSourceBlock::dateOutputsToStr(
		((const DateTimeSourceBlock*)block)->dateOutputs()));
}
