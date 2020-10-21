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

#include "VDIL/xml/MathExpBlockXmlParser.h"
#include "VDIL/blocks/MathExpBlock.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

bool MathExpBlockXmlParser::blockFromXml(BaseBlock *block,const QDomElement &blockElem,bool tryFixErrors)
{
	if(!blockElem.hasAttribute("expr")||!blockElem.hasAttribute("vars"))
		return tryFixErrors;
	QStringList vars=blockElem.attribute("vars").split("|");
	((MathExpBlock*)block)->setParams(vars,blockElem.attribute("expr"));
	return true;
}

void MathExpBlockXmlParser::blockToXml(const BaseBlock *block,QDomElement &blockElem)
{
	const MathExpBlock *b=(const MathExpBlock*)block;
	blockElem.setAttribute("expr",b->expr());
	blockElem.setAttribute("vars",b->vars().join('|'));
}
