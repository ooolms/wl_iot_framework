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

#include "VDIL/xml/SubProgramInternalBlocksParser.h"
#include "VDIL/core/SubProgram.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

bool SubProgramInternalInputBlockXmlParser::blockFromXml(BaseBlock *block,
	const QDomElement &blockElem,bool tryFixErrors)
{
	SubProgramInternalInputBlock *b=(SubProgramInternalInputBlock*)block;
	if(!blockElem.hasAttribute("sprg_output"))
		if(!tryFixErrors)return false;
	QString output=blockElem.attribute("sprg_output");
	TypeAndDim td=b->ownerSubProgram()->outputs().value(output);
	if(td.isValid())
		b->setSubProgramOutput(output,td);
	return true;
}

void SubProgramInternalInputBlockXmlParser::blockToXml(const BaseBlock *block,QDomElement &blockElem)
{
	const SubProgramInternalInputBlock *b=(const SubProgramInternalInputBlock*)block;
	blockElem.setAttribute("sprg_output",b->subProgramOutput());
}

bool SubProgramInternalOutputBlockXmlParser::blockFromXml(BaseBlock *block,
	const QDomElement &blockElem,bool tryFixErrors)
{
	SubProgramInternalOutputBlock *b=(SubProgramInternalOutputBlock*)block;
	if(!blockElem.hasAttribute("sprg_input"))
		if(!tryFixErrors)return false;
	QString input=blockElem.attribute("sprg_input");
	TypeAndDim td=b->ownerSubProgram()->inputs().value(input);
	if(td.isValid())
		b->setSubProgramInput(input,td);
	return true;
}

void SubProgramInternalOutputBlockXmlParser::blockToXml(const BaseBlock *block,QDomElement &blockElem)
{
	const SubProgramInternalOutputBlock *b=(const SubProgramInternalOutputBlock*)block;
	blockElem.setAttribute("sprg_input",b->subProgramInput());
}
