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

#include "VDIL/xml/VDevCommandSourceBlockXmlParser.h"
#include "VDIL/core/VDevCommandSourceBlock.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

bool VDevCommandSourceBlockXmlParser::blockFromXml(BaseBlock *block,const QDomElement &blockElem,bool tryFixErrors)
{
	if(!blockElem.hasAttribute("command"))
	{
		if(tryFixErrors)
			((VDevCommandSourceBlock*)block)->setCommand("");
		return tryFixErrors;
	}
	((VDevCommandSourceBlock*)block)->setCommand(blockElem.attribute("command").toUtf8());
	return true;
}

void VDevCommandSourceBlockXmlParser::blockToXml(const BaseBlock *block,QDomElement &blockElem)
{
	blockElem.setAttribute("command",QString::fromUtf8(((const VDevCommandSourceBlock*)block)->command()));
}
