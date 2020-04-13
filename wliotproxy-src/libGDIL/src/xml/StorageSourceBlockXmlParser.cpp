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

#include "GDIL/xml/StorageSourceBlockXmlParser.h"
#include "GDIL/blocks/StorageSourceBlock.h"

/*
	StorageId mStorId;
	QByteArray mDevName;
	quint32 mCount;
	SensorDef::Type mValType;
*/

bool StorageSourceBlockXmlParser::blockFromXml(BaseBlock *block,const QDomElement &blockElem)
{
	StorageSourceBlock *b=(StorageSourceBlock*)block;
	if(!blockElem.hasAttribute("dev_id")||!blockElem.hasAttribute("sensor_name")||!blockElem.hasAttribute("dev_name")||
		!blockElem.hasAttribute("count")||!blockElem.hasAttribute("values_type"))
			return false;
	StorageId stId;
	stId.deviceId=blockElem.attribute("dev_id");
	stId.sensorName=blockElem.attribute("sensor_name").toUtf8();
	QString devName=blockElem.attribute("dev_name");
	quint32 count=blockElem.attribute("count").toUInt();
	if(count==0)
		count=1;
	SensorDef::Type t;
	t.fromString(blockElem.attribute("values_type").toUtf8());
	b->setParams(stId,devName,t,count);
	return true;
}

void StorageSourceBlockXmlParser::blockToXml(const BaseBlock *block,QDomElement &blockElem)
{
	const StorageSourceBlock *b=(const StorageSourceBlock*)block;
	blockElem.setAttribute("dev_id",b->storageId().deviceId.toString());
	blockElem.setAttribute("sensor_name",QString::fromUtf8(b->storageId().sensorName));
	blockElem.setAttribute("dev_name",b->devName());
	blockElem.setAttribute("count",b->count());
	blockElem.setAttribute("values_type",QString::fromUtf8(b->valuesType().toString()));
}
