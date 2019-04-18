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

#include "CommandProcessor.h"
#include "wliot/storages/BaseFSSensorStorage.h"
#include "Commands/DevicesConfigCommand.h"
#include "Commands/DeviceIdCommand.h"
#include "Commands/ExecDeviceCommandCommand.h"
#include "Commands/GetSamplesCommand.h"
#include "Commands/IdentifyCommand.h"
#include "Commands/IdentifyTcpCommand.h"
#include "Commands/JSControlCommand.h"
#include "Commands/ListControlsCommand.h"
#include "Commands/ListIdentifiedCommand.h"
#include "Commands/ListSensorsCommand.h"
#include "Commands/RegisterVirtualDeviceCommand.h"
#include "Commands/SessionStorageCommands.h"
#include "Commands/StoragesCommands.h"
#include "Commands/SubscribeCommand.h"
#include "Commands/TtyCommands.h"
#include "Commands/AvailableDataExportServicesCommand.h"
#include "Commands/AccessCommand.h"
#include "Commands/DevNamesCommand.h"
#include "Commands/GetDevStateCommand.h"
#include "SysLogWrapper.h"
#include "MainServerConfig.h"
#include "wliot/WLIOTServerProtocolDefs.h"
#include "ServerInstance.h"
#include <QDebug>
#include <QEventLoop>

CommandProcessor::CommandProcessor(QtIODeviceWrap *d,bool forceRoot,QObject *parent)
	:QObject(parent)
{
	mUid=-1;
	if(forceRoot)
		mUid=0;
	inWork=false;
	needDeleteThis=false;
	mWasSync=true;
	dev=d;
	dev->setParent(this);
	syncTimer.setInterval(WLIOTProtocolDefs::syncWaitTime);
	syncTimer.setSingleShot(false);
	connect(dev,&QtIODeviceWrap::newMessage,this,&CommandProcessor::onNewMessage,Qt::DirectConnection);
	connect(ServerInstance::inst().devices(),&Devices::deviceIdentified,
		this,&CommandProcessor::onDeviceIdentified,Qt::DirectConnection);
	connect(ServerInstance::inst().devices(),&Devices::deviceDisconnected,
		this,&CommandProcessor::onDeviceLost,Qt::DirectConnection);
	connect(ServerInstance::inst().devices(),&Devices::deviceStateChanged,
		this,&CommandProcessor::onDeviceStateChanged,Qt::QueuedConnection);
	connect(ServerInstance::inst().storages(),&FSStoragesDatabase::storageCreated,
		this,&CommandProcessor::onStorageCreated,Qt::DirectConnection);
	connect(ServerInstance::inst().storages(),&FSStoragesDatabase::storageRemoved,
		this,&CommandProcessor::onStorageRemoved,Qt::DirectConnection);
	connect(&syncTimer,&QTimer::timeout,this,&CommandProcessor::onSyncTimer,Qt::QueuedConnection);

	addCommand(new DevicesConfigCommand(dev,this));
	addCommand(new DeviceIdCommand(dev,this));
	addCommand(new ExecDeviceCommandCommand(dev,this));
	addCommand(new GetSamplesCommand(dev,this));
	addCommand(new IdentifyCommand(dev,this));
	addCommand(new IdentifyTcpCommand(dev,this));
	addCommand(new JSControlCommand(dev,this));
	addCommand(new ListControlsCommand(dev,this));
	addCommand(new ListIdentifiedCommand(dev,this));
	addCommand(new ListSensorsCommand(dev,this));
	addCommand(new RegisterVirtualDeviceCommand(dev,this));
	addCommand(new SessionStorageCommands(dev,this));
	addCommand(new StoragesCommands(dev,this));
	addCommand(new SubscribeCommand(dev,this));
	addCommand(new TtyCommands(dev,this));
	addCommand(new AvailableDataExportServicesCommand(dev,this));
	addCommand(new AccessCommand(dev,this));
	addCommand(new DevNamesCommand(dev,this));
	addCommand(new GetDevStateCommand(dev,this));

	syncTimer.start();
}

CommandProcessor::~CommandProcessor()
{
	commandProcs.clear();
	for(ICommand *c:commands)
		delete c;
	for(VirtualDevice *d:vDevs)
	{
		d->disconnect(this);
		d->setClientPtr(0);
		d->setConnected(false);
	}
}

void CommandProcessor::scheduleDelete()
{
	if(!inWork)
		delete this;
	else needDeleteThis=true;
}

void CommandProcessor::registerVDevForCommandsProcessing(VirtualDevice *d)
{
	d->setClientPtr(this);
	vDevs[d->id()]=d;
	connect(d,&VirtualDevice::messageToDevice,this,&CommandProcessor::onMessageToVDev);
	connect(d,&VirtualDevice::disconnected,this,&CommandProcessor::onVDevDestroyed,Qt::QueuedConnection);
	if(!d->isConnected())
		d->setConnected(true);
}

IdType CommandProcessor::uid()
{
	return mUid;
}

void CommandProcessor::onNewValueWritten(const SensorValue *value)
{
	WorkLocker wLock(this);
	ISensorStorage *stor=(ISensorStorage*)sender();
	//user check was made when subscribed
//	if(accessMgr.userCanAccessDevice(stor->deviceId(),uid,DevicePolicyActionFlag::READ_STORAGES))
	dev->writeMsg(WLIOTProtocolDefs::measurementMsg,
		QByteArrayList()<<stor->deviceId().toByteArray()<<stor->sensor().name<<value->dumpToMsgArgs());
}

void CommandProcessor::onNewMessage(const Message &m)
{
	static const QSet<QByteArray> skippedMessages=QSet<QByteArray>()<<WLIOTProtocolDefs::funcAnswerOkMsg<<
		WLIOTProtocolDefs::funcAnswerErrMsg;
	if(skippedMessages.contains(m.title))return;
//	if(m.title==ARpcConfig::funcAnswerOkMsg||m.title==ARpcConfig::funcAnswerErrMsg||
//		m.title==vDevOkMsg||m.title==vDevErrMsg||m.title==ARpcConfig::funcSynccMsg)
//		return;
	WorkLocker wLock(this);
	if(m.title==WLIOTProtocolDefs::devSyncrMsg)
	{
		mWasSync=true;
		return;
	}
	if(m.args.count()<1)
	{
		qDebug()<<"invalid command";
		dev->writeMsg(WLIOTProtocolDefs::funcAnswerErrMsg,QByteArrayList()<<"invalid command");
		return;
	}
	QByteArray callId=m.args[0];
	if(m.title==WLIOTProtocolDefs::identifyMsg)
		dev->writeMsg(WLIOTProtocolDefs::funcAnswerOkMsg,
			QByteArrayList()<<callId<<MainServerConfig::serverId.toByteArray()<<MainServerConfig::serverName.toUtf8());
	else if(m.title==WLIOTServerProtocolDefs::vdevMsg)
	{
		if(m.args.count()<2)return;
		QUuid id(m.args[0]);
		if(id.isNull())return;
		if(vDevs.contains(id))
			vDevs[id]->onMessageFromDevice(Message(m.args[1],m.args.mid(2)));
		return;
	}
	else if(m.title==WLIOTServerProtocolDefs::authenticateSrvMsg)
	{
		if(m.args.count()<3)
		{
			if(mUid==rootUid&&m.args.count()==2)
			{
				QByteArray userName=m.args[1];
				IdType newUid=MainServerConfig::accessManager.userId(userName);
				if(newUid!=nullId)
				{
					//TODO add cleanup old user and uncomment
					qDebug()<<"authentification done";
					mUid=newUid;
					dev->writeMsg(WLIOTProtocolDefs::funcAnswerOkMsg,QByteArrayList()<<callId<<"authentification done");
					return;
				}
			}
			qDebug()<<"authentification failed";
			dev->writeMsg(WLIOTProtocolDefs::funcAnswerErrMsg,QByteArrayList()<<"authentification failed");
		}
		qDebug()<<"authentification in process";

		QByteArray userName=m.args[1];
		QByteArray pass=m.args[2];
		IdType newUid=MainServerConfig::accessManager.authentificateUser(userName,pass);
		if(newUid!=nullId)
		{
			if(mUid!=nullId&&mUid!=newUid)//TODO cleanup old user and remove this check (unsubscribe from vDevs, storages)
			{
				qDebug()<<"authentification failed";
				dev->writeMsg(WLIOTProtocolDefs::funcAnswerErrMsg,QByteArrayList()<<"authentification failed");
			}
			else
			{
				qDebug()<<"authentification done";
				mUid=newUid;
				dev->writeMsg(WLIOTProtocolDefs::funcAnswerOkMsg,QByteArrayList()<<callId<<"authentification done");
			}
		}
		else
		{
			qDebug()<<"authentification failed";
			dev->writeMsg(WLIOTProtocolDefs::funcAnswerErrMsg,QByteArrayList()<<callId<<"authentification failed");
		}
		return;
	}
	if(mUid==nullId)
		dev->writeMsg(WLIOTProtocolDefs::funcAnswerErrMsg,QByteArrayList()<<callId<<"authentification required");
	else
	{
		qDebug()<<"command from client: "<<m.title<<"; "<<m.args.join("|");
		if(commandProcs.contains(m.title))
		{
			bool clientDisconnected=false;
			auto conn=connect(dev,&QtIODeviceWrap::disconnected,[&clientDisconnected](){clientDisconnected=true;});
			ICommand *c=commandProcs[m.title];
			ICommand::CallContext ctx={m.title,callId,m.args.mid(1),QByteArrayList()};
			bool ok=c->processCommand(ctx);
			ctx.retVal.prepend(callId);
			if(!clientDisconnected)
			{
				if(ok)
				{
					qDebug()<<"ok answer: "<<ctx.retVal;
					dev->writeMsg(WLIOTProtocolDefs::funcAnswerOkMsg,ctx.retVal);
				}
				else
				{
					qDebug()<<"err answer: "<<ctx.retVal;
					dev->writeMsg(WLIOTProtocolDefs::funcAnswerErrMsg,ctx.retVal);
				}
			}
			if(conn)
				disconnect(conn);
		}
		else
			dev->writeMsg(WLIOTProtocolDefs::funcAnswerErrMsg,QByteArrayList()<<callId<<"unknown command"<<m.title);
	}
}

void CommandProcessor::onDeviceIdentified(QUuid id,QByteArray name,QUuid typeId)
{
	WorkLocker wLock(this);
	if(!MainServerConfig::accessManager.userCanAccessDevice(id,mUid,DevicePolicyActionFlag::ANY))
		return;
	dev->writeMsg(WLIOTServerProtocolDefs::notifyDeviceIdentifiedMsg,
		QByteArrayList()<<id.toByteArray()<<name<<typeId.toByteArray());
}

void CommandProcessor::onDeviceStateChanged(QUuid id,QByteArrayList args)
{
	if(!MainServerConfig::accessManager.userCanAccessDevice(id,mUid,DevicePolicyActionFlag::READ_STATE))
		return;
	dev->writeMsg(WLIOTProtocolDefs::stateChangedMsg,QByteArrayList()<<id.toByteArray()<<args);
}

void CommandProcessor::onDeviceLost(QUuid id)
{
	WorkLocker wLock(this);
	if(!MainServerConfig::accessManager.userCanAccessDevice(id,mUid,DevicePolicyActionFlag::ANY))
		return;
	dev->writeMsg(WLIOTServerProtocolDefs::notifyDeviceLostMsg,QByteArrayList()<<id.toByteArray());
}

void CommandProcessor::onStorageCreated(const StorageId &id)
{
	WorkLocker wLock(this);
	if(!MainServerConfig::accessManager.userCanAccessDevice(id.deviceId,mUid,DevicePolicyActionFlag::READ_STORAGES))
		return;
	ISensorStorage *st=ServerInstance::inst().storages()->existingStorage(id);
	if(!st)return;
	dev->writeMsg(WLIOTServerProtocolDefs::notifyStorageCreatedMsg,StoragesCommands::storageToMsgArguments(st));
}

void CommandProcessor::onStorageRemoved(const StorageId &id)
{
	WorkLocker wLock(this);
	if(!MainServerConfig::accessManager.userCanAccessDevice(id.deviceId,mUid,DevicePolicyActionFlag::READ_STORAGES))
		return;
	dev->writeMsg(WLIOTServerProtocolDefs::notifyStorageRemovedMsg,
		QByteArrayList()<<id.deviceId.toByteArray()<<id.sensorName);
}

void CommandProcessor::onMessageToVDev(const Message &m)
{
	VirtualDevice *d=(VirtualDevice*)sender();
	dev->writeMsg(WLIOTServerProtocolDefs::vdevMsg,QByteArrayList()<<d->id().toByteArray()<<m.title<<m.args);
}

void CommandProcessor::onVDevDestroyed()
{
	VirtualDevice *d=(VirtualDevice*)sender();
	vDevs.remove(d->id());
}

void CommandProcessor::onSyncTimer()
{
	if(mWasSync)
	{
		mWasSync=false;
		dev->writeMsg(WLIOTProtocolDefs::devSyncMsg);
	}
	else
	{
		syncTimer.stop();
		for(VirtualDevice *d:vDevs)
		{
			d->disconnect(this);
			d->setClientPtr(0);
			d->setConnected(false);
		}
		vDevs.clear();
		emit syncFailed();
	}
}

void CommandProcessor::addCommand(ICommand *c)
{
	commands.append(c);
	QByteArrayList cmds=c->acceptedCommands();
	for(QByteArray &cmd:cmds)
		commandProcs[cmd]=c;
}

CommandProcessor::WorkLocker::WorkLocker(CommandProcessor *p)
{
	proc=p;
	proc->inWork=true;
}

CommandProcessor::WorkLocker::~WorkLocker()
{
	proc->inWork=false;
	if(proc->needDeleteThis)
		delete proc;
}
