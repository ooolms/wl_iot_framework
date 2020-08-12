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
#include "Commands/AccessCommand.h"
#include "Commands/AlterozoomAuthCommand.h"
#include "Commands/AvailableDataExportServicesCommand.h"
#include "Commands/DeviceIdCommand.h"
#include "Commands/DevicesConfigCommand.h"
#include "Commands/DevNamesCommand.h"
#include "Commands/ExecDeviceCommandCommand.h"
#include "Commands/VDILControlCommand.h"
#include "Commands/GetDevStateCommand.h"
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
#include "Commands/TerminateCommand.h"
#include "SysLogWrapper.h"
#include "MainServerConfig.h"
#include "wliot/WLIOTServerProtocolDefs.h"
#include "ServerInstance.h"
#include <QDebug>
#include <QEventLoop>

//DBG
static int cliNum=0;

using namespace WLIOT;

CommandProcessor::CommandProcessor(QLocalSocket *s,QObject *parent)
	:QObject(parent)
{
	localClient=true;
	mUid=0;
	localSock=s;
	localSock->setParent(this);
	connect(localSock,SIGNAL(readyRead()),this,SLOT(onReadyRead()),Qt::DirectConnection);
	construct();
}

CommandProcessor::CommandProcessor(QSslSocket *s,QObject *parent)
	:QObject(parent)
{
	localClient=false;
	mUid=-1;
	netSock=s;
	netSock->setParent(this);
	connect(netSock,SIGNAL(readyRead()),this,SLOT(onReadyRead()),Qt::DirectConnection);
	construct();
}

CommandProcessor::~CommandProcessor()
{
	commandProcs.clear();
	for(ICommand *c:commands)
		delete c;
	for(VirtualDeviceBackend *be:QMap<QUuid,WLIOT::VirtualDeviceBackend*>(vDevs))
	{
		be->disconnect(this);
		be->forceDisconnect();
	}
	vDevs.clear();
}

void CommandProcessor::scheduleDelete()
{
	if(inWorkCommands==0)
		this->deleteLater();
	else needDeleteThis=true;
}

void CommandProcessor::registerVDev(VirtualDeviceBackend *d)
{
	if(d->msgCb()!=this)//paranoid mode
		return;
	vDevs[d->devId()]=d;
	connect(d,&VirtualDeviceBackend::destroyedBeforeQObject,this,
		&CommandProcessor::onVDevDestroyed);
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
	writeMsg(WLIOTProtocolDefs::measurementMsg,
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
//		qDebug()<<"syncr from client: "<<cliNum;
		mWasSync=true;
		return;
	}
	if(m.args.count()<1)
	{
		qDebug()<<"invalid command";
		writeMsg(WLIOTProtocolDefs::funcAnswerErrMsg,QByteArrayList()<<"invalid command");
		return;
	}
	QByteArray callId=m.args[0];
	if(m.title==WLIOTServerProtocolDefs::authenticateSrvMsg)
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
					writeMsg(WLIOTProtocolDefs::funcAnswerOkMsg,QByteArrayList()<<callId<<"authentification done");
					return;
				}
			}
			qDebug()<<"authentification failed";
			writeMsg(WLIOTProtocolDefs::funcAnswerErrMsg,QByteArrayList()<<"authentification failed");
			return;
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
				writeMsg(WLIOTProtocolDefs::funcAnswerErrMsg,QByteArrayList()<<"authentification failed");
			}
			else
			{
				qDebug()<<"authentification done";
				mUid=newUid;
				writeMsg(WLIOTProtocolDefs::funcAnswerOkMsg,QByteArrayList()<<callId<<"authentification done");
			}
		}
		else
		{
			qDebug()<<"authentification failed";
			writeMsg(WLIOTProtocolDefs::funcAnswerErrMsg,QByteArrayList()<<callId<<"authentification failed");
		}
		return;
	}
	if(mUid==nullId)
		writeMsg(WLIOTProtocolDefs::funcAnswerErrMsg,QByteArrayList()<<callId<<"authentification required");
	else if(m.title==WLIOTServerProtocolDefs::vdevMsg)
	{
		if(m.args.count()<2)return;
		QUuid id(m.args[0]);
		if(id.isNull())return;
		auto d=vDevs.value(id);
		if(d)d->emulateMessageFromDevice(Message(m.args[1],m.args.mid(2)));
		return;
	}
	else
	{
		if(commandProcs.contains(m.title))
		{
			ICommand *c=commandProcs[m.title];
			ICommand::CallContext ctx={m.title,callId,m.args.mid(1),QByteArrayList()};
			bool ok=c->processCommand(ctx);
			ctx.retVal.prepend(callId);
			if(ok)
			{
				writeMsg(WLIOTProtocolDefs::funcAnswerOkMsg,ctx.retVal);
			}
			else
			{
				writeMsg(WLIOTProtocolDefs::funcAnswerErrMsg,ctx.retVal);
			}
		}
		else
		{
			qDebug()<<"unknown command from client: "<<m.title;
			writeMsg(WLIOTProtocolDefs::funcAnswerErrMsg,QByteArrayList()<<callId<<"unknown command"<<m.title);
		}
	}
}

void CommandProcessor::onDeviceIdentified(QUuid id,QByteArray name,QUuid typeId)
{
	WorkLocker wLock(this);
	if(!MainServerConfig::accessManager.userCanAccessDevice(id,mUid,DevicePolicyActionFlag::ANY))
		return;
	writeMsg(WLIOTServerProtocolDefs::notifyDeviceIdentifiedMsg,
		QByteArrayList()<<id.toByteArray()<<name<<typeId.toByteArray());
}

void CommandProcessor::onDeviceStateChanged(QUuid id,QByteArrayList args)
{
	if(!MainServerConfig::accessManager.userCanAccessDevice(id,mUid,DevicePolicyActionFlag::READ_STATE))
		return;
	writeMsg(WLIOTProtocolDefs::stateChangedMsg,QByteArrayList()<<id.toByteArray()<<args);
}

void CommandProcessor::onDeviceLost(QUuid id)
{
	WorkLocker wLock(this);
	if(!MainServerConfig::accessManager.userCanAccessDevice(id,mUid,DevicePolicyActionFlag::ANY))
		return;
	writeMsg(WLIOTServerProtocolDefs::notifyDeviceLostMsg,QByteArrayList()<<id.toByteArray());
}

void CommandProcessor::onStorageCreated(const StorageId &id)
{
	WorkLocker wLock(this);
	if(!MainServerConfig::accessManager.userCanAccessDevice(id.deviceId,mUid,DevicePolicyActionFlag::READ_STORAGES))
		return;
	ISensorStorage *st=ServerInstance::inst().storages()->existingStorage(id);
	if(!st)return;
	writeMsg(WLIOTServerProtocolDefs::notifyStorageCreatedMsg,StoragesCommands::storageToMsgArguments(st));
}

void CommandProcessor::onStorageRemoved(const StorageId &id)
{
	WorkLocker wLock(this);
	if(!MainServerConfig::accessManager.userCanAccessDevice(id.deviceId,mUid,DevicePolicyActionFlag::READ_STORAGES))
		return;
	writeMsg(WLIOTServerProtocolDefs::notifyStorageRemovedMsg,
		QByteArrayList()<<id.deviceId.toByteArray()<<id.sensorName);
}

void CommandProcessor::onMessageToVDev(WLIOT::VirtualDeviceBackend *vDev,const Message &m)
{
	writeMsg(WLIOTServerProtocolDefs::vdevMsg,QByteArrayList()<<vDev->devId().toByteArray()<<m.title<<m.args);
}

void CommandProcessor::onVDevDestroyed()
{
	VirtualDeviceBackend *be=(VirtualDeviceBackend*)sender();
	vDevs.remove(be->devId());
}

void CommandProcessor::onSyncTimer()
{
	if(mWasSync)
	{
		mWasSync=false;
		writeMsg(WLIOTProtocolDefs::devSyncMsg);
//		qDebug()<<"send sync to client: "<<cliNum;
	}
	else
	{
		qDebug()<<"Client connection timeout: "<<cliNum;
//		writeMsg(WLIOTProtocolDefs::devSyncMsg);
//		qDebug()<<"send sync to client: "<<cliNum;
		syncTimer.stop();
		for(VirtualDeviceBackend *be:QMap<QUuid,WLIOT::VirtualDeviceBackend*>(vDevs))
		{
			be->disconnect(this);
			be->forceDisconnect();
		}
		vDevs.clear();
		emit syncFailed();
		if(localClient)
			localSock->disconnectFromServer();
		else netSock->disconnectFromHost();
	}
}

void CommandProcessor::onReadyRead()
{
	QByteArray data;
	if(localClient)
		data=localSock->readAll();
	else data=netSock->readAll();
	if(!data.isEmpty())
		QMetaObject::invokeMethod(this,"onNewData",Qt::QueuedConnection,Q_ARG(QByteArray,data));
}

void CommandProcessor::onNewData(QByteArray data)
{
	parser.pushData(data);
}

void CommandProcessor::construct()
{
	cliNum=::cliNum++;
	inWorkCommands=0;
	needDeleteThis=false;
	mWasSync=true;
	syncTimer.setInterval(WLIOTProtocolDefs::syncWaitTime);
	syncTimer.setSingleShot(false);
	connect(&parser,SIGNAL(newMessage(WLIOT::Message)),this,SLOT(onNewMessage(WLIOT::Message)),Qt::DirectConnection);
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

	addCommand(new AccessCommand(this));
	addCommand(new AlterozoomAuthCommand(this));
	addCommand(new AvailableDataExportServicesCommand(this));
	addCommand(new DevicesConfigCommand(this));
	addCommand(new DeviceIdCommand(this));
	addCommand(new DevNamesCommand(this));
	addCommand(new ExecDeviceCommandCommand(this));
	addCommand(new VDILControlCommand(this));
	addCommand(new GetDevStateCommand(this));
	addCommand(new GetSamplesCommand(this));
	addCommand(new IdentifyCommand(this));
	addCommand(new IdentifyTcpCommand(this));
	addCommand(new JSControlCommand(this));
	addCommand(new ListControlsCommand(this));
	addCommand(new ListIdentifiedCommand(this));
	addCommand(new ListSensorsCommand(this));
	addCommand(new RegisterVirtualDeviceCommand(this));
	addCommand(new SessionStorageCommands(this));
	addCommand(new StoragesCommands(this));
	addCommand(new SubscribeCommand(this));
	addCommand(new TtyCommands(this));
#ifdef DEBUG
	addCommand(new TerminateCommand(this));
#endif

	syncTimer.start();
	onReadyRead();

}

void CommandProcessor::writeMsg(const Message &m)
{
	QByteArray data=m.dump();
	writeData(data);
}

void CommandProcessor::writeMsg(const QByteArray &msg,const QByteArrayList &args)
{
	QByteArray data=Message::dump(msg,args);
	writeData(data);
}

void CommandProcessor::addCommand(ICommand *c)
{
	commands.append(c);
	QByteArrayList cmds=c->acceptedCommands();
	for(QByteArray &cmd:cmds)
		commandProcs[cmd]=c;
}

void CommandProcessor::writeData(const QByteArray &d)
{
	if(localClient)
	{
		if(localSock->isOpen())
		{
			localSock->write(d);
			localSock->flush();
		}
	}
	else if(netSock->isEncrypted())
	{
		netSock->write(d);
		netSock->flush();
	}
}

CommandProcessor::WorkLocker::WorkLocker(CommandProcessor *p)
{
	proc=p;
	++proc->inWorkCommands;
}

CommandProcessor::WorkLocker::~WorkLocker()
{
	--proc->inWorkCommands;
	if(proc->inWorkCommands==0&&proc->needDeleteThis)
		proc->deleteLater();
}
