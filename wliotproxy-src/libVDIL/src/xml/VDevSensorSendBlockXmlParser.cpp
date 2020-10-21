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

#include "VDIL/xml/VDevSensorSendBlockXmlParser.h"
#include "VDIL/core/VDevSensorSendBlock.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

bool VDevSensorSendBlockXmlParser::blockFromXml(BaseBlock *block,const QDomElement &blockElem,bool tryFixErrors)
{
	if(!blockElem.hasAttribute("sensor_name"))
	{
		if(tryFixErrors)
			((VDevSensorSendBlock*)block)->setSensorName("");
		return tryFixErrors;
	}
	((VDevSensorSendBlock*)block)->setSensorName(blockElem.attribute("sensor_name").toUtf8());
	return true;
}

void VDevSensorSendBlockXmlParser::blockToXml(const BaseBlock *block,QDomElement &blockElem)
{
	blockElem.setAttribute("sensor_name",QString::fromUtf8(((const VDevSensorSendBlock*)block)->sensorName()));
}
