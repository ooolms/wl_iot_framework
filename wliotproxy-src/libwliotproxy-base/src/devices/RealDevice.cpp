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

#include "wliot/devices/RealDevice.h"
#include "wliot/devices/CommandCall.h"
#include "wliot/devices/HubDeviceBackend.h"
#include <QTimer>
#include <QEventLoop>
#include <QDebug>

using namespace WLIOT;

static const int syncWaitIntervals=4;

RealDevice::RealDevice(QObject *parent)
	:QObject(parent)
{
	callId=0;
	mBackend=nullptr;
	mSyncCounter=4;
	hubDevice=false;
	syncTimer.setInterval(WLIOTProtocolDefs::syncWaitTime);
	syncTimer.setSingleShot(false);
	mControlsLoaded=mSensorsLoaded=mStateLoaded=false;
	mConnected=false;
	identifyCall=new CommandCall(WLIOTProtocolDefs::identifyMsg,this);
	identifyCall->setupTimer(WLIOTProtocolDefs::identifyWaitTime)->setUseCallMsg(false)->setRecallOnDevReset(true);
	connect(&syncTimer,&QTimer::timeout,this,&RealDevice::onSyncTimer,Qt::QueuedConnection);
}

RealDevice::~RealDevice()
{
	stopCommands(CommandCall::DEV_DESTROYED);
	mBackend->disconnect(this);
	delete mBackend;
}

void RealDevice::setBackend(IHighLevelDeviceBackend *b)
{
	if(mBackend)
		delete mBackend;
	mBackend=b;
	mBackend->setParent(this);
	connect(b,SIGNAL(newMessageFromDevice(WLIOT::Message)),this,SLOT(onNewMessage(WLIOT::Message)));
	connect(b,&IHighLevelDeviceBackend::connected,this,&RealDevice::onConnected);
	connect(b,&IHighLevelDeviceBackend::disconnected,this,&RealDevice::onDisconnected);
	connect(b,&IHighLevelDeviceBackend::deviceReset,this,&RealDevice::onDeviceReset);
	connect(b,&IHighLevelDeviceBackend::destroyed,this,&RealDevice::onBackendDestroyed);
	if(mBackend->isConnected())
		onConnected();
}

IHighLevelDeviceBackend* RealDevice::takeBackend()
{
	if(!mBackend)return nullptr;
	stopCommands(CommandCall::DEV_DESTROYED);
	mBackend->disconnect(this);
	mBackend->setParent(nullptr);
	IHighLevelDeviceBackend *r=mBackend;
	mBackend=nullptr;
	return r;
}

IHighLevelDeviceBackend* RealDevice::backend()
{
	return mBackend;
}

RealDevice::IdentifyResult RealDevice::identify()
{
	if(!isConnected()||identifyCall->isWorking()||!devId.isNull())
		return FAILED;
	devId=QUuid();
	devName.clear();
	identifyCall->run(this,"0");
	identifyCall->wait();
	QByteArrayList retVal=identifyCall->returnValue();
	if(!identifyCall->ok()||retVal.count()<2)
		return FAILED;
	bool tmpHubDeviceFlag=(retVal[0]==WLIOTProtocolDefs::hubMsg);
	if(tmpHubDeviceFlag)
	{
		retVal.removeAt(0);
		if(retVal.count()<2)
			return FAILED;
	}
	hubDevice=tmpHubDeviceFlag;
	if(retVal[1].isEmpty())
		return OK_NULL_ID_OR_NAME;
	QUuid newId;
	QByteArray newName;
	if(retVal[0].startsWith('{'))
		newId=QUuid(retVal[0]);
	else newId=QUuid::fromRfc4122(QByteArray::fromHex(retVal[0]));
	if(newId.isNull())
		return OK_NULL_ID_OR_NAME;
	newName=retVal[1];
	QUuid newTypeId;
	if(retVal.count()>2)
	{
		if(retVal[2].startsWith('{'))
			newTypeId=QUuid(retVal[2]);
		else newTypeId=QUuid::fromRfc4122(QByteArray::fromHex(retVal[2]));

	}
	devId=newId;
	devName=newName;
	devTypeId=newTypeId;
	emit identified();
	return OK;
}

//void RealDevice::resetIdentification(QUuid newId,QByteArray newName,QUuid newTypeId)
//{
//	mSensorsLoaded=mControlsLoaded=mStateLoaded=false;
//	std::swap(devId,newId);
//	std::swap(devName,newName);
//	std::swap(devTypeId,newTypeId);
//	emit identificationChanged(newId,devId);
//	if(!devId.isNull()&&!syncTimer.isActive())
//	{
//		mSyncCounter=syncWaitIntervals;
//		syncTimer.start();
//	}
//}

void RealDevice::onConnected()
{
	if(mConnected)return;
	mConnected=true;
	emit connected();
	onDeviceReset();
	if(devId.isNull())
		identify();
}

void RealDevice::onDisconnected()
{
	if(!mConnected)return;
	mConnected=false;
	mSensorsLoaded=mControlsLoaded=mStateLoaded=false;
	mSyncCounter=0;
	syncTimer.stop();
	stopCommands(CommandCall::DEV_DISCONNECTED);
	emit disconnected();
}

void RealDevice::onNewMessage(const Message &m)
{
	if(hubDevice&&m.title==WLIOTProtocolDefs::hubMsg)
		onHubMsg(m);
	else if(m.title==WLIOTProtocolDefs::devSyncrMsg)
		mSyncCounter=syncWaitIntervals;
	else if(m.title==WLIOTProtocolDefs::deviceInfoMsg)
		identifyCall->onOkMsg(m.args);
	else if(m.title==WLIOTProtocolDefs::funcAnswerOkMsg)
	{
		if(m.args.isEmpty())return;
		const QByteArray &callIdStr=m.args[0];
		CommandCall *cmd=execCommands.value(callIdStr,QSharedPointer<CommandCall>()).data();
		if(!cmd)return;
		cmd->onOkMsg(m.args.mid(1));
	}
	else if(m.title==WLIOTProtocolDefs::funcAnswerErrMsg)
	{
		if(m.args.isEmpty())return;
		const QByteArray &callIdStr=m.args[0];
		CommandCall *cmd=execCommands.value(callIdStr,QSharedPointer<CommandCall>()).data();
		if(!cmd)return;
		cmd->onErrorMsg(m.args.mid(1));
	}
	else if(m.title==WLIOTProtocolDefs::stateChangedMsg)
	{
		if((m.args.count()%3)!=0)return;
		for(int i=0;i<m.args.count()/3;++i)
		{
			QByteArray command=m.args[3*i];
			QByteArray nameOrIndex=m.args[3*i+1];
			QByteArray value=m.args[3*i+2];
			if(command.isEmpty())continue;
			else if(command=="#")
			{
				if(nameOrIndex.isEmpty())continue;
				mState.additionalAttributes[nameOrIndex]=value;
			}
			else
			{
				bool ok=false;
				quint32 index=nameOrIndex.toUInt(&ok);
				if(!ok)continue;
				mState.commandParams[command][index]=value;
			}
		}
		emit stateChanged(m.args);
	}
	else emit newMessageFromDevice(m);
}

void RealDevice::onDeviceReset()
{
	mStateLoaded=false;
	stopCommands(CommandCall::DEV_RESET);
	emit deviceWasReset();
	mSyncCounter=syncWaitIntervals;
	syncTimer.stop();
	syncTimer.start();
}

void RealDevice::onSyncTimer()
{
	if(mSyncCounter)
	{
		--mSyncCounter;
		writeMsgToDevice(WLIOTProtocolDefs::devSyncMsg);
	}
	else
	{
		emit syncFailed();
		mBackend->forceDisconnect();
		onDisconnected();
			//just to be sure, if forceDisconnect() already
			//has emited disconnected() signal, all will be ok
	}
}

void RealDevice::onCommandDone()
{
	CommandCall *call=(CommandCall*)sender();
	execCommands.remove(call->callId());
}

void RealDevice::onBackendDestroyed()
{
	onDisconnected();
	mBackend=0;
}

void RealDevice::onChildDeviceDisconnectedForced()
{
	HubDeviceBackend *b=(HubDeviceBackend*)sender();
	if(hubDevicesMap.contains(b->mId))
		delete hubDevicesMap.take(b->mId);
}

void RealDevice::onHubMsg(const Message &m)
{
	if(m.args.count()<2)return;
	QUuid id;
	if(m.args[0].startsWith('{'))
		id=QUuid(m.args[0]);
	else id=QUuid::fromRfc4122(QByteArray::fromHex(m.args[0]));
	if(id.isNull()||m.args[1].isEmpty())return;
	if(m.args[1]==WLIOTProtocolDefs::hubDeviceIdentifiedMsg)
	{
		id=QUuid(m.args[1]);
		if(m.args.count()<3)return;
		QUuid typeId;
		if(m.args.count()>=4)
			typeId=QUuid(m.args[3]);
		onHubDeviceIdentified(id,m.args[2],typeId);
	}
	else if(m.args[1]==WLIOTProtocolDefs::hubDeviceLostMsg)
	{
		if(hubDevicesMap.contains(id))
			delete hubDevicesMap.take(id);
	}
	else
	{
		if(!hubDevicesMap.contains(id))return;
		Message m2=m;
		m2.title=m.args[1];
		m2.args.removeFirst();
		m2.args.removeFirst();
		hubDevicesMap[id]->messageFromDevice(m2);
	}
}

void RealDevice::onHubDeviceIdentified(const QUuid &id,const QByteArray &name,const QUuid &typeId)
{
	HubDeviceBackend *b=hubDevicesMap.value(id);
	if(!b)
	{
		b=new HubDeviceBackend(id,name,typeId,this);
		hubDevicesMap[id]=b;
		connect(b,&HubDeviceBackend::forceDisconnectCalled,this,&RealDevice::onChildDeviceDisconnectedForced);
	}
	else b->setSelfConnected(true);
	emit childDeviceIdentified(id);
}

void RealDevice::stopCommands(CommandCall::Error err)
{
	if(!mBackend)return;
	for(auto cmd:execCommands.values())
	{
		cmd->onError(err);
		cmd->deleteLater();
	}
	if(identifyCall->isWorking())
		identifyCall->onError(err);
}

bool RealDevice::identifyHub()
{
	if(!isConnected()||devId.isNull())return false;
	return execCommand((new CommandCall(WLIOTProtocolDefs::identifyHubCommand))->
		setUseCallMsg(false)->setupTimer(WLIOTProtocolDefs::identifyWaitTime))->wait();
}

bool RealDevice::isConnected()const
{
	return mConnected;
}

QSharedPointer<CommandCall> RealDevice::execCommand(CommandCall *call)
{
	QByteArray callIdStr=QByteArray::number(++callId);
	QSharedPointer<CommandCall> callPtr(call);
	execCommands[callIdStr]=callPtr;
	connect(call,&CommandCall::done,this,&RealDevice::onCommandDone,Qt::QueuedConnection);
	if(!isConnected())
		call->onErrorMsg("device disconnected");
	else call->run(this,callIdStr);
	return callPtr;
}

QSharedPointer<CommandCall> RealDevice::execCommand(const QByteArray &cmd)
{
	return execCommand(new CommandCall(cmd));
}

QSharedPointer<CommandCall> RealDevice::execCommand(const QByteArray &cmd,const QByteArrayList &args)
{
	return execCommand((new CommandCall(cmd))->setArgs(args));
}

bool RealDevice::isReady()
{
	return mConnected&&!devId.isNull();
}

QUuid RealDevice::id()
{
	return devId;
}

QUuid RealDevice::classId()
{
	return devTypeId;
}

QByteArray RealDevice::name()
{
	return devName;
}

void RealDevice::renameDevice(const QByteArray &newName,bool silent)
{
	if(devName==newName)
		return;
	devName=newName;
	if(!silent)
		emit nameChanged(devName);
}

bool RealDevice::getSensorsDescription(QList<SensorDef> &sensors)
{
	if(mSensorsLoaded)
	{
		sensors=mSensors;
		return true;
	}
	QSharedPointer<CommandCall> call=execCommand((new CommandCall(WLIOTProtocolDefs::getSensorsCommand))->
		setupTimer(WLIOTProtocolDefs::syncWaitTime*4));
	if(!call->wait())return false;
	QByteArrayList retVal=call->returnValue();
	if(retVal.count()<1)return false;
	retVal[0]=retVal[0].trimmed();
	QList<SensorDef> newSensors;
	if(retVal[0].startsWith('{'))
	{
		if(!SensorDef::parseJsonDescription(retVal[0],newSensors))return false;
	}
	else if(!SensorDef::parseXmlDescription(retVal[0],newSensors))return false;
	mSensors=newSensors;
	mSensorsLoaded=true;
	sensors=mSensors;
	return true;
}

bool RealDevice::getControlsDescription(ControlsGroup &controls)
{
	if(mControlsLoaded)
	{
		controls=mControls;
		return true;
	}
	QSharedPointer<CommandCall> call=execCommand((new CommandCall(WLIOTProtocolDefs::getControlsCommand))->
		setupTimer(WLIOTProtocolDefs::syncWaitTime*4));
	if(!call->wait())return false;
	QByteArrayList retVal=call->returnValue();
	if(retVal.count()<1)return false;
	retVal[0]=retVal[0].trimmed();
	ControlsGroup newControls;
	if(retVal[0].startsWith('{'))
	{
		if(!ControlsGroup::parseJsonDescription(retVal[0],newControls))return false;
	}
	else if(!ControlsGroup::parseXmlDescription(retVal[0],newControls))return false;
	mControls=newControls;
	mControlsLoaded=true;
	controls=mControls;
	return true;
}

bool RealDevice::getState(DeviceState &state)
{
	if(mStateLoaded)
	{
		state=mState;
		return true;
	}
	QSharedPointer<CommandCall> call=execCommand((new CommandCall(WLIOTProtocolDefs::getStateCommand))->
		setupTimer(WLIOTProtocolDefs::syncWaitTime*4));
	if(!call->wait())return false;
	QByteArrayList retVal=call->returnValue();
	if(!mState.parseMsgArgs(retVal))return false;
	mStateLoaded=true;
	state=mState;
	return true;
}

bool RealDevice::writeMsgToDevice(const Message &m)
{
	if(!mBackend)return false;
	return mBackend->writeMessageToDevice(m);
}

bool RealDevice::isHubDevice()
{
	return hubDevice;
}

QList<QUuid> RealDevice::childDevices()
{
	return hubDevicesMap.keys();
}

HubDeviceBackend* RealDevice::childDevice(const QUuid &id)
{
	return hubDevicesMap.value(id);
}
