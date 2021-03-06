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

#include "VDIL/xml/CommandBlockXmlParser.h"
#include "VDIL/blocks/CommandBlock.h"
#include "wliot/devices/StreamParser.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

bool CommandBlockXmlParser::blockFromXml(BaseBlock *block,const QDomElement &blockElem,bool tryFixErrors)
{
	CommandBlock *b=(CommandBlock*)block;
	if(!blockElem.hasAttribute("dev_id")||!blockElem.hasAttribute("cmd")||
		!blockElem.hasAttribute("args")||!blockElem.hasAttribute("inputs_count")||
		!blockElem.hasAttribute("enable_condition_input"))
		if(!tryFixErrors)return false;
	QUuid devId=QUuid(blockElem.attribute("dev_id"));
	QByteArray cmd=blockElem.attribute("cmd").toUtf8();
	Message argsMsg;
	StreamParser::tryParse(WLIOTProtocolDefs::argDelim+blockElem.attribute("args").toUtf8()+
		WLIOTProtocolDefs::msgDelim,argsMsg);
	quint32 inputsCount=blockElem.attribute("inputs_count").toUInt();
	CommandBlock::OutputMode m=CommandBlock::outModeFromStr(blockElem.attribute("out_mode"));
	b->setParams(devId,cmd,argsMsg.args,inputsCount,blockElem.attribute("enable_condition_input")=="1",m);
	return true;
}

void CommandBlockXmlParser::blockToXml(const BaseBlock *block,QDomElement &blockElem)
{
	const CommandBlock *b=(const CommandBlock*)block;
	blockElem.setAttribute("dev_id",b->deviceId().toString());
	blockElem.setAttribute("cmd",QString::fromUtf8(b->cmd()));
	QByteArray args=Message::dump("",b->args()).mid(1);
	args.chop(1);
	blockElem.setAttribute("args",QString::fromUtf8(args));
	blockElem.setAttribute("inputs_count",b->argInputsCount());
	blockElem.setAttribute("enable_condition_input",QString::fromUtf8(b->enableConditionInput()?"1":"0"));
	blockElem.setAttribute("out_mode",CommandBlock::outModeToStr(b->outMode()));
}
