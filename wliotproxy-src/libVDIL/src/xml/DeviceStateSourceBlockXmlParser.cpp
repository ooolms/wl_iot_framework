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

#include "VDIL/xml/DeviceStateSourceBlockXmlParser.h"
#include "VDIL/blocks/DeviceStateSourceBlock.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

bool DeviceStateSourceBlockXmlParser::blockFromXml(BaseBlock *block,const QDomElement &blockElem)
{
	DeviceStateSourceBlock *b=(DeviceStateSourceBlock*)block;
	if(!blockElem.hasAttribute("dev_id")||!blockElem.hasAttribute("state_key")||
		!blockElem.hasAttribute("is_command_state")||!blockElem.hasAttribute("bool_out")||
		!blockElem.hasAttribute("command_state_index"))
		return false;
	QUuid devId(blockElem.attribute("dev_id"));
	QByteArray stateKey=blockElem.attribute("state_key").toUtf8();
	bool cmdState=blockElem.attribute("is_command_state")=="1";
	bool boolOut=blockElem.attribute("bool_out")=="1";
	quint32 cmdStateIndex=blockElem.attribute("command_state_index").toUInt();
	b->setParams(devId,stateKey,cmdState,boolOut,cmdStateIndex);
	return true;
}

void DeviceStateSourceBlockXmlParser::blockToXml(const BaseBlock *block,QDomElement &blockElem)
{
	const DeviceStateSourceBlock *b=(const DeviceStateSourceBlock*)block;
	blockElem.setAttribute("dev_id",b->deviceId().toString());
	blockElem.setAttribute("state_key",QString::fromUtf8(b->stateKey()));
	blockElem.setAttribute("is_command_state",QString::fromUtf8(b->commandState()?"1":"0"));
	blockElem.setAttribute("bool_out",QString::fromUtf8(b->boolOut()?"1":"0"));
	blockElem.setAttribute("command_state_index",b->commandStateIndex());
}
