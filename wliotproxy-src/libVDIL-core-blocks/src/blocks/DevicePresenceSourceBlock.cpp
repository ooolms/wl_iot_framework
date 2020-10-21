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

#include "VDIL/blocks/DevicePresenceSourceBlock.h"
#include "VDIL/core/CoreBlocksGroupFactory.h"
#include <QThread>

using namespace WLIOT;
using namespace WLIOTVDIL;

const QString DevicePresenceSourceBlock::mBlockName=QString("device_presence");

DevicePresenceSourceBlock::DevicePresenceSourceBlock(quint32 blockId)
	:SourceBlock(blockId)
{
	triggerOut=mkOutput(TypeAndDim(DataUnit::BOOL,1),"trigger out");
	connectedOut=mkOutput(TypeAndDim(DataUnit::BOOL,1),"connected out");
}

QString DevicePresenceSourceBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString DevicePresenceSourceBlock::blockName()const
{
	return mBlockName;
}

QUuid DevicePresenceSourceBlock::deviceId()const
{
	return mDevId;
}

void DevicePresenceSourceBlock::setDeviceId(const QUuid &devId)
{
	mDevId=devId;
}

DataUnit WLIOTVDIL::DevicePresenceSourceBlock::extractDataInternal()
{
	RealDevice *dev=helper()->devById(mDevId);
	if(!dev)return DataUnit(false);
	return DataUnit(dev->isConnected());
}

void WLIOTVDIL::DevicePresenceSourceBlock::evalInternal(const DataUnit &data,bool triggerActivated)
{
	triggerOut->setData(DataUnit(triggerActivated));
	connectedOut->setData(data);
}
