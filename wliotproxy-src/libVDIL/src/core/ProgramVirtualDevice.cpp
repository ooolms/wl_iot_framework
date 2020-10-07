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
#include "VDIL/core/CoreBlocksGroupFactory.h"
#include "VDIL/blocks/VDevSensorSendBlock.h"
#include "VDIL/blocks/VDevCommandSourceBlock.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

ProgramVirtualDevice::ProgramVirtualDevice(Program *p)
{
	prg=p;
	mEnabled=false;
}

//void ProgramVirtualDevice::onMessageToVDev(WLIOT::VirtualDeviceBackend *vDev,const WLIOT::Message &m)
//{
//	if(m.title==WLIOTProtocolDefs::identifyMsg)
//		vDev->emulateMessageFromDevice(Message(WLIOTProtocolDefs::deviceInfoMsg,
//			QByteArrayList()<<prg->vdev()->devId().toByteArray()<<prg->vdev()->devName()));
//	else if(m.title==WLIOTProtocolDefs::devSyncMsg)
//		vDev->emulateMessageFromDevice(Message(WLIOTProtocolDefs::devSyncrMsg,m.args));
//	else if(m.title==WLIOTProtocolDefs::funcCallMsg)
//	{
//		QByteArray callIdStr;
//		if(m.args.count()<2||m.args[0].size()==0||m.args[1].size()==0)
//		{
//			vDev->emulateMessageFromDevice(Message(WLIOTProtocolDefs::funcAnswerErrMsg,
//				QByteArrayList()<<""<<"No command or call id"));
//			return;
//		}
//		callIdStr=m.args[0];
//		if(m.args[1][0]=='#')
//		{
//			if(m.args[1]==WLIOTProtocolDefs::getSensorsCommand)
//			{
//				QByteArray data;
//				SensorsParser::dumpToXml(data,prg->vdev()->sensors());
//				vDev->emulateMessageFromDevice(Message(WLIOTProtocolDefs::funcAnswerOkMsg,
//					QByteArrayList()<<callIdStr<<data));
//			}
//			else if(m.args[1]==WLIOTProtocolDefs::getControlsCommand)
//			{
//				QByteArray data;
//				ControlsParser::dumpToXml(data,prg->vdev()->controls());
//				vDev->emulateMessageFromDevice(Message(WLIOTProtocolDefs::funcAnswerOkMsg,
//					QByteArrayList()<<callIdStr<<data));
//			}
//			else if(m.args[1]==WLIOTProtocolDefs::getStateCommand)
//				vDev->emulateMessageFromDevice(Message(WLIOTProtocolDefs::funcAnswerOkMsg,
//					QByteArrayList()<<callIdStr<<prg->vdev()->state().dumpToMsgArgs()));
//			else vDev->emulateMessageFromDevice(Message(WLIOTProtocolDefs::funcAnswerErrMsg,
//				QByteArrayList()<<callIdStr<<"bad system command"));
//		}
//		else
//		{
//			QByteArrayList retVal;
//			QByteArray cmd=m.args[1];
//			QByteArrayList cmdArgs=m.args.mid(2);
//			if(!mCommands.contains(cmd))
//			{
//				vDev->emulateMessageFromDevice(Message(WLIOTProtocolDefs::funcAnswerErrMsg,
//					QByteArrayList()<<callIdStr<<"unknown command"));
//				return;
//			}
//			if(mCommands[cmd].params.count()!=cmdArgs.count())
//			{
//				vDev->emulateMessageFromDevice(Message(WLIOTProtocolDefs::funcAnswerErrMsg,
//					QByteArrayList()<<callIdStr<<"invalid command parameters"));
//				return;
//			}
//			vDev->emulateMessageFromDevice(Message(WLIOTProtocolDefs::funcAnswerOkMsg,QByteArrayList()<<callIdStr));
//			auto &map=mState.commandParams[cmd];
//			map.clear();
//			for(int i=0;i<cmdArgs.count();++i)
//				map[(quint32)i]=cmdArgs[i];
//			for(VDevCommandSourceBlock *b:mCmdBlocksMap.value(cmd))
//				b->setTriggerActivated();
//			emit activateProgram();
//		}
//	}
//}

const QList<WLIOT::SensorDef>& ProgramVirtualDevice::sensors()const
{
	return mSensors;
}

WLIOT::SensorDef ProgramVirtualDevice::sensorByName(const QByteArray &name)const
{
	for(const auto &s:mSensors)
	{
		if(s.name!=name)continue;
		return s;
	}
	return WLIOT::SensorDef();
}

void ProgramVirtualDevice::setSensors(const QList<WLIOT::SensorDef> &sensors)
{
	mSensors=sensors;
	for(BaseBlock *b:prg->allBlocks())
	{
		if(b->groupName()!=CoreBlocksGroupFactory::mGroupName)continue;
		if(b->blockName()==VDevSensorSendBlock::mBlockName)
		{
			VDevSensorSendBlock *bb=(VDevSensorSendBlock*)b;
			bb->updateInput();
		}
	}
}

const WLIOT::ControlsGroup& ProgramVirtualDevice::controls()const
{
	return mControls;
}

const QMap<QByteArray,ControlsCommand>& ProgramVirtualDevice::commandsMap()const
{
	return mCommands;
}

void ProgramVirtualDevice::setParams(bool enabled,const QUuid &id,const QByteArray &name,const QUuid &typeId)
{
	mEnabled=enabled;
	mDevId=id;
	mDevName=name;
	mTypeId=typeId;
}

QUuid ProgramVirtualDevice::devId()const
{
	return mDevId;
}

QByteArray ProgramVirtualDevice::devName()const
{
	return mDevName;
}

QUuid ProgramVirtualDevice::typeId()const
{
	return mTypeId;
}

bool ProgramVirtualDevice::enabled()const
{
	return mEnabled;
}

const WLIOT::DeviceState& ProgramVirtualDevice::state()const
{
	return mState;
}

void ProgramVirtualDevice::prepareToStart()
{
	for(BaseBlock *b:prg->allBlocks())
	{
		if(b->groupName()!=CoreBlocksGroupFactory::mGroupName)continue;
		if(b->blockName()==VDevCommandSourceBlock::mBlockName)
		{
			VDevCommandSourceBlock *bb=(VDevCommandSourceBlock*)b;
			if(bb->command().isEmpty())continue;
			mCmdBlocksMap[bb->command()].append(bb);
		}
	}
}

void ProgramVirtualDevice::cleanupAfterStop()
{
	mCmdBlocksMap.clear();
}

bool ProgramVirtualDevice::onCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)
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

void ProgramVirtualDevice::setControls(const WLIOT::ControlsGroup &controls)
{
	mControls=controls;
	mCommands=controls.extractCommandsMap();
	mState=DeviceState::makeFromCommands(mControls.extractCommandsList());
	for(BaseBlock *b:prg->allBlocks())
	{
		if(b->groupName()!=CoreBlocksGroupFactory::mGroupName)continue;
		if(b->blockName()==VDevCommandSourceBlock::mBlockName)
		{
			VDevCommandSourceBlock *bb=(VDevCommandSourceBlock*)b;
			bb->setCommand(bb->command());
		}
	}
}
