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

#include "VDIL/xml/RuntimeSourceBlockXmlParser.h"
#include "VDIL/blocks/RuntimeSourceBlock.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

bool RuntimeSourceBlockXmlParser::blockFromXml(BaseBlock *block,const QDomElement &blockElem,bool tryFixErrors)
{
	if(!blockElem.hasAttribute("var_name"))
	{
		if(tryFixErrors)
			((RuntimeSourceBlock*)block)->setVarName("");
		return tryFixErrors;
	}
	((RuntimeSourceBlock*)block)->setVarName(blockElem.attribute("var_name"));
	return true;
}

void RuntimeSourceBlockXmlParser::blockToXml(const BaseBlock *block,QDomElement &blockElem)
{
	blockElem.setAttribute("var_name",((const RuntimeSourceBlock*)block)->varName());
}
