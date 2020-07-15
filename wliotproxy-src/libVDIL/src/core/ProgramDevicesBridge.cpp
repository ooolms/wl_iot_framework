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

#include "VDIL/core/ProgramDevicesBridge.h"
#include "VDIL/core/CoreBlocksGroupFactory.h"
#include "VDIL/blocks/DevicePresenceSourceBlock.h"
#include "VDIL/blocks/DeviceStateSourceBlock.h"
#include "VDIL/core/Program.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

ProgramDevicesBridge::ProgramDevicesBridge(WLIOTVDIL::Program *p)
{
	prg=p;
}

void ProgramDevicesBridge::prepareToStart()
{
	for(BaseBlock *b:prg->allBlocks())
	{
		if(b->groupName()!=CoreBlocksGroupFactory::mGroupName)continue;
		if(b->blockName()==DevicePresenceSourceBlock::mBlockName)
		{
			DevicePresenceSourceBlock *bb=(DevicePresenceSourceBlock*)b;
			QUuid devId=bb->deviceId();
			if(devId.isNull())continue;
			mDevPresenceBlocksMap[devId].append(bb);
			if(mDevPresenceConnMap.contains(devId))continue;
			RealDevice *dev=prg->helper()->devById(devId);
			if(!dev)continue;
			QPair<QMetaObject::Connection,QMetaObject::Connection> t;
			t.first=connect(dev,&RealDevice::connected,this,&ProgramDevicesBridge::onDeviceConnectedAndDisconnected);
			t.second=connect(dev,&RealDevice::disconnected,this,
				&ProgramDevicesBridge::onDeviceConnectedAndDisconnected);
			mDevPresenceConnMap[devId]=t;
		}
		else if(b->blockName()==DeviceStateSourceBlock::mBlockName)
		{
			DeviceStateSourceBlock *bb=(DeviceStateSourceBlock*)b;
			QUuid devId=bb->deviceId();
			if(devId.isNull())continue;
			mDevStateBlocksMap[devId].append(bb);
			if(mDevStateConnMap.contains(devId))continue;
			RealDevice *dev=prg->helper()->devById(devId);
			if(!dev)continue;
			mDevStateConnMap[devId]=connect(dev,&RealDevice::stateChanged,
				this,&ProgramDevicesBridge::onDeviceStateChanged);
		}
	}
}

void ProgramDevicesBridge::cleanupAfterStop()
{
	for(QMetaObject::Connection &c:mDevStateConnMap)
		disconnect(c);
	for(QPair<QMetaObject::Connection,QMetaObject::Connection> &c:mDevPresenceConnMap)
	{
		disconnect(c.first);
		disconnect(c.second);
	}
	mDevStateConnMap.clear();
	mDevStateBlocksMap.clear();
	mDevPresenceConnMap.clear();
	mDevPresenceBlocksMap.clear();
}

void ProgramDevicesBridge::onDeviceConnectedAndDisconnected()
{
	RealDevice *dev=(RealDevice*)sender();
	for(DevicePresenceSourceBlock *b:mDevPresenceBlocksMap.value(dev->id()))
		b->setTriggerActivated();
	emit activateProgram();
}

void ProgramDevicesBridge::onDeviceStateChanged()
{
	RealDevice *dev=(RealDevice*)sender();
	for(DeviceStateSourceBlock *b:mDevStateBlocksMap.value(dev->id()))
		b->setTriggerActivated();
	emit activateProgram();
}
