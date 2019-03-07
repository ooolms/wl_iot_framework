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

#include "IotProxyCommandProcessor.h"
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
#include "Commands/VdevMeasCommand.h"
#include "Commands/AvailableDataExportServicesCommand.h"
#include "Commands/ChangeDeviceOwnerCommand.h"
#include "Commands/VdevMeasbCommand.h"
#include "Commands/AccessCommand.h"
#include "SysLogWrapper.h"
#include "IotProxyConfig.h"
#include "wliot/ServerConfig.h"
#include "IotProxyConfig.h"
#include "IotProxyInstance.h"
#include <QDebug>
#include <QEventLoop>

const QByteArray IotProxyCommandProcessor::vDevOkMsg="vdev_ok";
const QByteArray IotProxyCommandProcessor::vDevErrMsg="vdev_err";

IotProxyCommandProcessor::IotProxyCommandProcessor(QtIODeviceWrap *d,bool forceRoot,QObject *parent)
	:QObject(parent)
{
	mUid=-1;
	if(forceRoot)
		mUid=0;
	inWork=false;
	needDeleteThis=false;
	dev=d;
	dev->setParent(this);
	connect(dev,&QtIODeviceWrap::newMessage,this,&IotProxyCommandProcessor::onNewMessage,Qt::DirectConnection);
	connect(IotProxyInstance::inst().devices(),&IotProxyDevices::deviceIdentified,
		this,&IotProxyCommandProcessor::onDeviceIdentified,Qt::DirectConnection);
	connect(IotProxyInstance::inst().devices(),&IotProxyDevices::deviceDisconnected,
		this,&IotProxyCommandProcessor::onDeviceLost,Qt::DirectConnection);
	connect(IotProxyInstance::inst().devices(),&IotProxyDevices::deviceStateChanged,
		this,&IotProxyCommandProcessor::onDeviceStateChanged,Qt::QueuedConnection);
	connect(IotProxyInstance::inst().storages(),&FSStoragesDatabase::storageCreated,
		this,&IotProxyCommandProcessor::onStorageCreated,Qt::DirectConnection);
	connect(IotProxyInstance::inst().storages(),&FSStoragesDatabase::storageRemoved,
		this,&IotProxyCommandProcessor::onStorageRemoved,Qt::DirectConnection);

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
	addCommand(new VdevMeasCommand(dev,this));
	addCommand(new VdevMeasbCommand(dev,this));
	addCommand(new AvailableDataExportServicesCommand(dev,this));
	addCommand(new ChangeDeviceOwnerCommand(dev,this));
	addCommand(new AccessCommand(dev,this));
}

IotProxyCommandProcessor::~IotProxyCommandProcessor()
{
	commandProcs.clear();
	for(ICommand *c:commands)
		delete c;
}

void IotProxyCommandProcessor::scheduleDelete()
{
	if(!inWork)delete this;
	else needDeleteThis=true;
}

void IotProxyCommandProcessor::registerVDevForCommandsProcessing(VirtualDevice *d)
{
	if(vDevs.contains(d))return;
	connect(d,&VirtualDevice::processDeviceCommand,this,&IotProxyCommandProcessor::onProcessCommandFromVDev);
	connect(d,&VirtualDevice::destroyed,[this,d]()
	{
		vDevs.removeOne(d);
	});
	vDevs.append(d);
}

IdType IotProxyCommandProcessor::uid()
{
	return mUid;
}

void IotProxyCommandProcessor::onNewValueWritten(const SensorValue *value)
{
	inWork=true;
	ISensorStorage *stor=(ISensorStorage*)sender();
	//user check was made when subscribed
//	if(accessMgr.userCanAccessDevice(stor->deviceId(),uid,DevicePolicyActionFlag::READ_STORAGES))
	dev->writeMsg(WLIOTConfig::measurementMsg,
		QByteArrayList()<<stor->deviceId().toByteArray()<<stor->sensor().name<<value->dumpToMsgArgs());
	inWork=false;
	if(needDeleteThis)
		delete this;
}

void IotProxyCommandProcessor::onNewMessage(const Message &m)
{
	static const QSet<QByteArray> skippedMessages=QSet<QByteArray>()<<WLIOTConfig::funcAnswerOkMsg<<
		WLIOTConfig::funcAnswerErrMsg<<vDevOkMsg<<vDevErrMsg<<WLIOTConfig::funcSynccMsg;
	if(skippedMessages.contains(m.title))return;
//	if(m.title==ARpcConfig::funcAnswerOkMsg||m.title==ARpcConfig::funcAnswerErrMsg||
//		m.title==vDevOkMsg||m.title==vDevErrMsg||m.title==ARpcConfig::funcSynccMsg)
//		return;
	inWork=true;
	if(m.args.count()<1)
	{
		qDebug()<<"invalid command";
		dev->writeMsg(WLIOTConfig::funcAnswerErrMsg,QByteArrayList()<<"invalid command");
		inWork=false;
		if(needDeleteThis)
			delete this;
		return;
	}
	QByteArray callId=m.args[0];
	if(m.title==WLIOTConfig::identifyMsg)
		dev->writeMsg(WLIOTConfig::funcAnswerOkMsg,
			QByteArrayList()<<callId<<IotProxyConfig::serverId.toByteArray()<<IotProxyConfig::serverName.toUtf8());
	else if(m.title==ServerConfig::authenticateSrvMsg)
	{
		if(m.args.count()<3)
		{
			if(mUid==rootUid&&m.args.count()==2)
			{
				QByteArray userName=m.args[1];
				IdType newUid=IotProxyConfig::accessManager.userId(userName);
				if(newUid!=nullId)
				{
					//TODO add cleanup old user and uncomment
					qDebug()<<"authentification done";
					mUid=newUid;
					dev->writeMsg(WLIOTConfig::funcAnswerOkMsg,QByteArrayList()<<callId<<"authentification done");
					return;
				}
			}
			qDebug()<<"authentification failed";
			dev->writeMsg(WLIOTConfig::funcAnswerErrMsg,QByteArrayList()<<"authentification failed");
		}
		qDebug()<<"authentification in process";

		QByteArray userName=m.args[1];
		QByteArray pass=m.args[2];
		IdType newUid=IotProxyConfig::accessManager.authentificateUser(userName,pass);
		if(newUid!=nullId)
		{
			if(mUid!=nullId&&mUid!=newUid)//TODO cleanup old user and remove this check (unsubscribe from vDevs, storages)
			{
				qDebug()<<"authentification failed";
				dev->writeMsg(WLIOTConfig::funcAnswerErrMsg,QByteArrayList()<<"authentification failed");
			}
			else
			{
				qDebug()<<"authentification done";
				mUid=newUid;
				dev->writeMsg(WLIOTConfig::funcAnswerOkMsg,QByteArrayList()<<callId<<"authentification done");
			}
		}
		else
		{
			qDebug()<<"authentification failed";
			dev->writeMsg(WLIOTConfig::funcAnswerErrMsg,QByteArrayList()<<callId<<"authentification failed");
		}
		inWork=false;
		if(needDeleteThis)
			delete this;
		return;
	}
	if(mUid==nullId)
		dev->writeMsg(WLIOTConfig::funcAnswerErrMsg,QByteArrayList()<<callId<<"authentification required");
	else
	{
		qDebug()<<"command from client: "<<m.title<<"; "<<m.args.join("|");
		if(commandProcs.contains(m.title))
		{
			ICommand *c=commandProcs[m.title];
			ICommand::CallContext ctx={m.title,callId,m.args.mid(1),QByteArrayList()};
			bool ok=c->processCommand(ctx);
			ctx.retVal.prepend(callId);
			if(ok)
			{
				qDebug()<<"ok answer: "<<ctx.retVal;
				dev->writeMsg(WLIOTConfig::funcAnswerOkMsg,ctx.retVal);
			}
			else
			{
				qDebug()<<"err answer: "<<ctx.retVal;
				dev->writeMsg(WLIOTConfig::funcAnswerErrMsg,ctx.retVal);
			}
		}
		else
			dev->writeMsg(WLIOTConfig::funcAnswerErrMsg,QByteArrayList()<<callId<<"unknown command"<<m.title);
	}
	inWork=false;
	if(needDeleteThis)
		delete this;
}

void IotProxyCommandProcessor::onDeviceIdentified(QUuid id,QByteArray name,QByteArray type)
{
	if(!IotProxyConfig::accessManager.userCanAccessDevice(id,mUid,DevicePolicyActionFlag::ANY))
		return;
	dev->writeMsg(ServerConfig::notifyDeviceIdentifiedMsg,QByteArrayList()<<id.toByteArray()<<name<<type);
}

void IotProxyCommandProcessor::onDeviceStateChanged(QUuid id,QByteArrayList args)
{
	if(!IotProxyConfig::accessManager.userCanAccessDevice(id,mUid,DevicePolicyActionFlag::READ_STATE))
		return;
	dev->writeMsg(WLIOTConfig::stateChangedMsg,QByteArrayList()<<id.toByteArray()<<args);
}

void IotProxyCommandProcessor::onDeviceLost(QUuid id)
{
	if(!IotProxyConfig::accessManager.userCanAccessDevice(id,mUid,DevicePolicyActionFlag::ANY))
		return;
	dev->writeMsg(ServerConfig::notifyDeviceLostMsg,QByteArrayList()<<id.toByteArray());
}

void IotProxyCommandProcessor::onStorageCreated(const StorageId &id)
{
	if(!IotProxyConfig::accessManager.userCanAccessDevice(id.deviceId,mUid,DevicePolicyActionFlag::READ_STORAGES))
		return;
	ISensorStorage *st=IotProxyInstance::inst().storages()->existingStorage(id);
	if(!st)return;
	dev->writeMsg(ServerConfig::notifyStorageCreatedMsg,StoragesCommands::storageToMsgArguments(st));
}

void IotProxyCommandProcessor::onStorageRemoved(const StorageId &id)
{
	if(!IotProxyConfig::accessManager.userCanAccessDevice(id.deviceId,mUid,DevicePolicyActionFlag::READ_STORAGES))
		return;
	dev->writeMsg(ServerConfig::notifyStorageRemovedMsg,QByteArrayList()<<id.deviceId.toByteArray()<<id.sensorName);
}

void IotProxyCommandProcessor::onProcessCommandFromVDev(
	const QByteArray &cmd,const QByteArrayList &args,bool &ok,QByteArrayList &retVal)
{
	VirtualDevice *vDev=(VirtualDevice*)sender();
	QByteArray callId=QByteArray::number(qrand());
	QTimer t;
	t.setInterval(WLIOTConfig::synccCallWaitTime);
	t.setSingleShot(true);
	ok=false;
	bool done=false;
	QEventLoop loop;
	auto conn=connect(dev,&QtIODeviceWrap::newMessage,
		[this,&retVal,&ok,&t,&loop,&done,&callId,&vDev](const Message &m)
	{
		if(m.args.isEmpty())return;
		if(m.title==vDevOkMsg&&m.args[0]==callId)
		{
			ok=true;
			done=true;
			retVal=m.args.mid(1);
			loop.quit();
		}
		else if(m.title==vDevErrMsg&&m.args[0]==callId)
		{
			done=true;
			retVal=m.args.mid(1);
			loop.quit();
		}
		else if(m.title==WLIOTConfig::funcSynccMsg&&m.args[0]==callId)
		{
			vDev->writeCommandCallSync(callId);
			t.stop();
			t.start();
		}
	});
	dev->writeMsg("vdev_command",QByteArrayList()<<callId<<vDev->id().toByteArray()<<cmd<<args);
	t.start();
	if(!done)loop.exec(QEventLoop::ExcludeUserInputEvents);
	disconnect(conn);
}

void IotProxyCommandProcessor::addCommand(ICommand *c)
{
	commands.append(c);
	QByteArrayList cmds=c->acceptedCommands();
	for(QByteArray &cmd:cmds)
		commandProcs[cmd]=c;
}
