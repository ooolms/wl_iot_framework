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

#include "VDIL/core/ProgramDevicesRuntime.h"
#include "VDIL/core/CoreBlocksGroupFactory.h"
#include "VDIL/blocks/DevicePresenceSourceBlock.h"
#include "VDIL/blocks/DeviceStateSourceBlock.h"
#include "VDIL/core/Program.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

ProgramDevicesRuntimeInstance::ProgramDevicesRuntimeInstance(WLIOTVDIL::Program *p)
	:IProgramRuntimeInstance(p)
{
}

void ProgramDevicesRuntimeInstance::prepareToStart()
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
			t.first=connect(dev,&RealDevice::connected,this,&ProgramDevicesRuntimeInstance::onDeviceConnectedAndDisconnected);
			t.second=connect(dev,&RealDevice::disconnected,this,
				&ProgramDevicesRuntimeInstance::onDeviceConnectedAndDisconnected);
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
				this,&ProgramDevicesRuntimeInstance::onDeviceStateChanged);
		}
	}
}

void ProgramDevicesRuntimeInstance::cleanupAfterStop()
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

void ProgramDevicesRuntimeInstance::onDeviceConnectedAndDisconnected()
{
	RealDevice *dev=(RealDevice*)sender();
	for(DevicePresenceSourceBlock *b:mDevPresenceBlocksMap.value(dev->id()))
		b->setTriggerActivated();
	emit activateProgram();
}

void ProgramDevicesRuntimeInstance::onDeviceStateChanged(const QByteArrayList &args)
{
	RealDevice *dev=(RealDevice*)sender();
	DeviceState st;
	st.parseMsgArgs(args);
	bool hasAffectedBlocks=false;
	for(DeviceStateSourceBlock *b:mDevStateBlocksMap.value(dev->id()))
	{
		if(b->commandState())
		{
			if(st.commandParams.contains(b->stateKey())&&
				st.commandParams[b->stateKey()].contains(b->commandStateIndex()))
			{
				b->setTriggerActivated();
				b->setValFromTriggerSlot(st.commandParams[b->stateKey()][b->commandStateIndex()]);
				hasAffectedBlocks=true;
			}
		}
		else if(st.additionalAttributes.contains(b->stateKey()))
		{
			b->setTriggerActivated();
			b->setValFromTriggerSlot(st.additionalAttributes[b->stateKey()]);
			hasAffectedBlocks=true;
		}
	}
	if(hasAffectedBlocks)
		emit activateProgram();
}

IProgramRuntimeInstance *WLIOTVDIL::ProgramDevicesRuntime::mkRuntime(Program *p)
{
	return new ProgramDevicesRuntimeInstance(p);
}
