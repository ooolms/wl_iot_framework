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
static const int maxSyncCount=4;

using namespace WLIOT;

CommandProcessor::CommandProcessor(QLocalSocket *s,QObject *parent)
	:QObject(parent)
{
	connType=UnixSock;
	if(MainServerConfig::unixSocketNeedsAuth)
		mUid=nullId;
	else mUid=rootUid;
	localSock=s;
	localSock->setParent(this);
	connect(localSock,SIGNAL(readyRead()),this,SLOT(onReadyRead()),Qt::DirectConnection);
	construct();
}

CommandProcessor::CommandProcessor(QSslSocket *s,QObject *parent)
	:QObject(parent)
{
	connType=TcpSock;
	mUid=nullId;
	netSock=s;
	netSock->setParent(this);
	connect(netSock,SIGNAL(readyRead()),this,SLOT(onReadyRead()),Qt::DirectConnection);
	construct();
}

CommandProcessor::CommandProcessor(QProcess *p,QObject *parent)
:	QObject(parent)
{
	connType=ChildProc;
	mUid=rootUid;
	childProc=p;
	connect(childProc,SIGNAL(readyRead()),this,SLOT(onReadyRead()),Qt::DirectConnection);
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

bool CommandProcessor::hasVDev(const QUuid &id)
{
	return vDevs.contains(id);
}

void CommandProcessor::disconnectVDev(const QUuid &id)
{
	VirtualDeviceBackend *be=vDevs.value(id);
	if(be)
		be->forceDisconnect();
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
	//"syncr" msg - no need to answer
	if(m.title==WLIOTProtocolDefs::devSyncrMsg)
	{
		mSyncCount=maxSyncCount;
		return;
	}

	//get call id
	if(m.args.count()<1)
	{
		qDebug()<<"invalid command";
		writeMsg(WLIOTProtocolDefs::funcAnswerErrMsg,QByteArrayList()<<"invalid command");
		return;
	}
	QByteArray callId=m.args[0];

	//check if user is not authenticated
	if(mUid==nullId)
	{
		if(m.title==WLIOTProtocolDefs::identifyMsg)
		{
			writeMsg(Message(WLIOTProtocolDefs::funcAnswerOkMsg,QByteArrayList()<<callId<<
				MainServerConfig::serverId.toByteArray()<<MainServerConfig::serverName.toUtf8()));
		}
		else if(m.title==WLIOTServerProtocolDefs::authenticateCmd)
			authenticate(m);
		else writeMsg(WLIOTProtocolDefs::funcAnswerErrMsg,QByteArrayList()<<callId<<"authentication required");
		return;
	}

	/*	static const QSet<QByteArray> skippedMessages=QSet<QByteArray>()<<WLIOTProtocolDefs::funcAnswerOkMsg<<
		WLIOTProtocolDefs::funcAnswerErrMsg;
	if(skippedMessages.contains(m.title))return;*/

	if(m.title==WLIOTServerProtocolDefs::vdevMsg)
	{
		WorkLocker wLock(this);
		if(m.args.count()<2)return;
		QUuid id(m.args[0]);
		if(id.isNull())return;
		auto d=vDevs.value(id);
		if(d)d->emulateMessageFromDevice(Message(m.args[1],m.args.mid(2)));
		return;
	}
	else if(m.title==WLIOTServerProtocolDefs::authenticateCmd)
		authenticate(m);//change user
	else if(m.title==WLIOTServerProtocolDefs::userInfoCmd)
		writeMsg(WLIOTProtocolDefs::funcAnswerOkMsg,QByteArrayList()<<callId<<QByteArray::number(mUid)<<
			MainServerConfig::accessManager.userName(mUid));
	else
	{
		if(commandProcs.contains(m.title))
		{
			WorkLocker wLock(this);
			ICommand *c=commandProcs[m.title];
			ICommand::CallContext ctx={m.title,callId,m.args.mid(1),QByteArrayList()};
			bool ok=c->processCommand(ctx);
			ctx.retVal.prepend(callId);
			if(ok)
				writeMsg(WLIOTProtocolDefs::funcAnswerOkMsg,ctx.retVal);
			else writeMsg(WLIOTProtocolDefs::funcAnswerErrMsg,ctx.retVal);
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
	if(mSubscribedStorages.contains(id))
		disconnect(mSubscribedStorages.take(id));
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

void CommandProcessor::subscribeOnStorage(ISensorStorage *st)
{
	if(mSubscribedStorages.contains(st->id()))return;
	QMetaObject::Connection c=connect(st,SIGNAL(newValueWritten(const WLIOT::SensorValue*)),
		this,SLOT(onNewValueWritten(const WLIOT::SensorValue*)));
	mSubscribedStorages[st->id()]=c;
}

void CommandProcessor::unsubscribeFromStorage(ISensorStorage *st)
{
	if(!mSubscribedStorages.contains(st->id()))return;
	disconnect(mSubscribedStorages.take(st->id()));
}

void CommandProcessor::onVDevDestroyed()
{
	VirtualDeviceBackend *be=(VirtualDeviceBackend*)sender();
	vDevs.remove(be->devId());
}

void CommandProcessor::onSyncTimer()
{
	--mSyncCount;
	if(mSyncCount==0)
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
		if(connType==UnixSock)
			localSock->disconnectFromServer();
		else if(connType==TcpSock)
			netSock->disconnectFromHost();
		else
		{
			childProc->close();
			childProc->terminate();
		}
	}
	else writeMsg(WLIOTProtocolDefs::devSyncMsg);
}

void CommandProcessor::onReadyRead()
{
	QByteArray data;
	if(connType==UnixSock)
		data=localSock->readAll();
	else if(connType==TcpSock)
		data=netSock->readAll();
	else data=childProc->readAll();
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
	mSyncCount=maxSyncCount;
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
	if(connType==UnixSock)
	{
		if(localSock->isOpen())
		{
			localSock->write(d);
			localSock->flush();
		}
	}
	else if(connType==TcpSock)
	{
		if(netSock->isEncrypted())
		{
			netSock->write(d);
			netSock->flush();
		}
	}
	else childProc->write(d);
}

void CommandProcessor::authenticate(const Message &m)
{
	QByteArray callId=m.args[0];
	if(!vDevs.isEmpty()||inWorkCommands!=0||!mSubscribedStorages.isEmpty())
	{
		writeMsg(WLIOTProtocolDefs::funcAnswerErrMsg,QByteArrayList()<<"has virtual devices, executing commands or "
			"opened storages, can't change user now");
		return;
	}
	qDebug()<<"authentification in process";
	QByteArray userName=m.args[1];
	QByteArray pass=m.args.value(2);
	if(mUid==rootUid)
	{
		qDebug()<<"user change from root does not require to check password";
		IdType newUid=MainServerConfig::accessManager.userId(userName);
		if(newUid==nullId)
		{
			qDebug()<<"no user found";
			writeMsg(WLIOTProtocolDefs::funcAnswerErrMsg,QByteArrayList()<<"no user found");
		}
		else
		{
			mUid=newUid;
			writeMsg(WLIOTProtocolDefs::funcAnswerOkMsg,QByteArrayList()<<callId<<QByteArray::number(mUid));
		}
		return;
	}
	IdType newUid=MainServerConfig::accessManager.authenticateUser(userName,pass);
	if(newUid!=nullId)
	{
		qDebug()<<"authentication done";
		mUid=newUid;
		writeMsg(WLIOTProtocolDefs::funcAnswerOkMsg,QByteArrayList()<<callId<<QByteArray::number(mUid));
	}
	else
	{
		qDebug()<<"authentication failed";
		writeMsg(WLIOTProtocolDefs::funcAnswerErrMsg,QByteArrayList()<<callId<<"authentication failed");
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
