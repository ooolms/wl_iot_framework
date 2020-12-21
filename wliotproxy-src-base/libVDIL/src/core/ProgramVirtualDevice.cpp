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

#include "VDIL/core/ProgramVirtualDevice.h"
#include "VDIL/core/Program.h"
#include "VDIL/core/VDevSensorSendBlock.h"
#include "VDIL/core/VDevCommandSourceBlock.h"
#include "VDIL/core/VDevAdditionalStateChangeBlock.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

ProgramVirtualDeviceRuntimeInstance::ProgramVirtualDeviceRuntimeInstance(Program *p)
	:IProgramRuntimeInstance(p)
{
	mEnabled=false;
}

const QList<WLIOT::SensorDef>& ProgramVirtualDeviceRuntimeInstance::sensors()const
{
	return mSensors;
}

WLIOT::SensorDef ProgramVirtualDeviceRuntimeInstance::sensorByName(const QByteArray &name)const
{
	for(const auto &s:mSensors)
	{
		if(s.name!=name)continue;
		return s;
	}
	return WLIOT::SensorDef();
}

void ProgramVirtualDeviceRuntimeInstance::setSensors(const QList<WLIOT::SensorDef> &sensors)
{
	mSensors=sensors;
	for(BaseBlock *b:prg->allBlocks())
	{
		if(b->groupName()!=Program::reservedCoreGroupName)continue;
		if(b->blockName()==VDevSensorSendBlock::mBlockName)
		{
			VDevSensorSendBlock *bb=(VDevSensorSendBlock*)b;
			bb->updateInput();
		}
	}
}

const WLIOT::ControlsGroup& ProgramVirtualDeviceRuntimeInstance::controls()const
{
	return mControls;
}

const QMap<QByteArray,ControlsCommand>& ProgramVirtualDeviceRuntimeInstance::commandsMap()const
{
	return mCommands;
}

void ProgramVirtualDeviceRuntimeInstance::setParams(bool enabled,const QUuid &id,const QByteArray &name,const QUuid &typeId)
{
	mEnabled=enabled;
	mDevId=id;
	mDevName=name;
	mTypeId=typeId;
}

QUuid ProgramVirtualDeviceRuntimeInstance::devId()const
{
	return mDevId;
}

QByteArray ProgramVirtualDeviceRuntimeInstance::devName()const
{
	return mDevName;
}

QUuid ProgramVirtualDeviceRuntimeInstance::typeId()const
{
	return mTypeId;
}

bool ProgramVirtualDeviceRuntimeInstance::enabled()const
{
	return mEnabled;
}

const WLIOT::DeviceState& ProgramVirtualDeviceRuntimeInstance::state()const
{
	return mState;
}

const DeviceState& ProgramVirtualDeviceRuntimeInstance::startupState()const
{
	return mStartupState;
}

void ProgramVirtualDeviceRuntimeInstance::prepareToStart()
{
	mState=mStartupState;
	for(BaseBlock *b:prg->allBlocks())
	{
		if(b->groupName()!=Program::reservedCoreGroupName)continue;
		if(b->blockName()==VDevCommandSourceBlock::mBlockName)
		{
			VDevCommandSourceBlock *bb=(VDevCommandSourceBlock*)b;
			if(bb->command().isEmpty())continue;
			mCmdBlocksMap[bb->command()].append(bb);
		}
	}
}

void ProgramVirtualDeviceRuntimeInstance::cleanupAfterStop()
{
	mCmdBlocksMap.clear();
}

bool ProgramVirtualDeviceRuntimeInstance::onCommand(const QByteArray &cmd,
	const QByteArrayList &args,QByteArrayList &retVal)
{
	if(!mCommands.contains(cmd))
	{
		retVal.append("unknown command");
		return false;
	}
	if(mCommands[cmd].params.count()!=args.count())
	{
		retVal.append("invalid command parameters");
		return false;
	}
	auto &map=mState.commandParams[cmd];
	map.clear();
	for(int i=0;i<args.count();++i)
		map[(quint32)i]=args[i];
	for(VDevCommandSourceBlock *b:mCmdBlocksMap.value(cmd))
		b->setTriggerActivated();
	emit activateProgram();
	return true;
}

void ProgramVirtualDeviceRuntimeInstance::setControls(const WLIOT::ControlsGroup &controls)
{
	mControls=controls;
	mCommands=controls.extractCommandsMap();
	mState=DeviceState::makeFromCommands(mControls.extractCommandsList());
	//remove lost commands from startupState
	for(QByteArray &cmd:mStartupState.commandParams.keys())
		if(!mState.commandParams.contains(cmd))
			mStartupState.commandParams.remove(cmd);
	//add new commands to startupState
	for(auto i=mState.commandParams.begin();i!=mState.commandParams.end();++i)
	{
		auto &newPMap=i.value();
		auto &oldPMap=mStartupState.commandParams[i.key()];
		//remove lost params from command
		for(quint32 pIndex:oldPMap.keys())
			if(!newPMap.contains(pIndex))
				oldPMap.remove(pIndex);
		//add new params to command
		for(auto j=newPMap.begin();j!=newPMap.end();++j)
			if(!oldPMap.contains(j.key()))
				oldPMap[j.key()]=j.value();
	}
	mState=mStartupState;
	for(BaseBlock *b:prg->allBlocks())
	{
		if(b->groupName()!=Program::reservedCoreGroupName)continue;
		if(b->blockName()==VDevCommandSourceBlock::mBlockName)
		{
			VDevCommandSourceBlock *bb=(VDevCommandSourceBlock*)b;
			bb->setCommand(bb->command());
		}
	}
}

void ProgramVirtualDeviceRuntimeInstance::setStartupState(const DeviceState &st)
{
	mStartupState=st;
	//remove unnecessary commands
	for(QByteArray &cmd:mStartupState.commandParams.keys())
	{
		if(!mCommands.contains(cmd))
			mStartupState.commandParams.remove(cmd);
	}
	//TODO check command params
	//update VDevAdditionalStateChangeBlock blocks
	for(BaseBlock *b:prg->allBlocks())
	{
		if(b->groupName()!=Program::reservedCoreGroupName)continue;
		if(b->blockName()==VDevAdditionalStateChangeBlock::mBlockName)
		{
			VDevAdditionalStateChangeBlock *bb=(VDevAdditionalStateChangeBlock*)b;
			bb->setStateParam(bb->stateParam());
		}
	}
}
