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

#include "wliot/client/ServerConnection.h"
#include "wliot/client/StoragesCommands.h"
#include "wliot/client/ServerCommandCall.h"
#include "ServerConnectionSocketWrap.h"
#include <QDebug>
#include <QEventLoop>
#include <QCoreApplication>

using namespace WLIOT;
using namespace WLIOTClient;

ServerConnection::ServerConnection(QObject *parent)
	:QObject(parent)
{
	noDebug=false;
	sock=0;
	callIdNum=0;
	wasSyncMsg=false;
	ready=false;
	proxy=QNetworkProxy(QNetworkProxy::NoProxy);
	sockThread=new QThread(this);
	syncTimer.setInterval(WLIOTProtocolDefs::syncWaitTime*2);
	syncTimer.setSingleShot(false);
	connect(&parser,&StreamParser::newMessage,this,&ServerConnection::onRawMessage);
	connect(&syncTimer,&QTimer::timeout,this,&ServerConnection::onSyncTimer,Qt::QueuedConnection);
}

ServerConnection::~ServerConnection()
{
	if(isConnected())
	{
		disconnectFromServer();
		onDevDisconnected();
	}
	if(sockThread->isRunning())
	{
		sockThread->quit();
		sockThread->wait(3000);
		sockThread->terminate();
		sockThread->wait(200);
	}
	delete sockThread;
}

bool ServerConnection::startConnectLocal()
{
	if(sock)return false;
	netConn=false;
	ready=false;
	parser.reset();
	sockThread->start();
	while(!sockThread->isRunning())
		QThread::yieldCurrentThread();
	sock=new ServerConnectionSocketWrap(this);
	sock->moveToThread(sockThread);
	QMetaObject::invokeMethod(sock,"startConnectLocal",Qt::QueuedConnection);
	return true;
}

void ServerConnection::setNoDebug(bool n)
{
	noDebug=n;
}

bool ServerConnection::startConnectNet(const QString &host,quint16 port)
{
	if(sock)return false;
	mHost=host;
	mPort=port;
	netConn=true;
	ready=false;
	parser.reset();
	sockThread->start();
	while(!sockThread->isRunning())
		QThread::yieldCurrentThread();
	sock=new ServerConnectionSocketWrap(this);
	sock->moveToThread(sockThread);
	QMetaObject::invokeMethod(sock,"startConnectNet",Qt::QueuedConnection);
	return true;
}

bool ServerConnection::authenticateNet(const QByteArray &userName,const QByteArray &pass)
{
	if(!sock->netSock->isEncrypted())
		return false;
	QByteArrayList retVal;
	bool oldNetAuth=ready;
	ready=true;//if not this, call will not be processed
	if(!execCommand(WLIOTServerProtocolDefs::authenticateSrvMsg,QByteArrayList()<<userName<<pass,retVal))
		return false;
	ready=oldNetAuth;
	bool ok=false;
	uid=retVal.value(0).toLong(&ok);
	if(!ok)
		uid=-1;
	if(!ready)
	{
		ready=true;
		emit preconnected();
		qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
		emit connected();
		wasSyncMsg=true;
		syncTimer.start();
	}
	emit authenticationChanged();
	return true;
}

bool ServerConnection::authenticateLocalFromRoot(const QByteArray &userName)
{
	if(!sock->localSock->isOpen())return false;
	QByteArrayList retVal;
	if(!execCommand(WLIOTServerProtocolDefs::authenticateSrvMsg,QByteArrayList()<<userName,retVal))
		return false;
	bool ok=false;
	uid=retVal.value(0).toLong(&ok);
	if(!ok)
		uid=-1;
	emit authenticationChanged();
	return true;
}

bool ServerConnection::isConnected()
{
	return ready;
}

bool ServerConnection::execCommand(const QByteArray &cmd,
	const QByteArrayList &args,QByteArrayList &retVal,CmDataCallback onCmData)
{
	ServerCommandCall call(this,onCmData,cmd,args);
	call.call();
	retVal=call.returnValue();
	return call.ok();
}

bool ServerConnection::execCommand(const QByteArray &cmd,const QByteArrayList &args,CmDataCallback onCmData)
{
	QByteArrayList retVal;
	return execCommand(cmd,args,retVal,onCmData);
}

bool ServerConnection::waitForConnected(int msec)
{
	if(!sock)return false;
	if(isConnected())
		return true;
	QEventLoop loop;
	QTimer t;
	t.setInterval(msec);
	t.setSingleShot(true);
	connect(this,&ServerConnection::connected,&loop,&QEventLoop::quit);
	connect(this,&ServerConnection::connectionError,&loop,&QEventLoop::quit);
	connect(&t,&QTimer::timeout,&loop,&QEventLoop::quit);
		loop.exec(QEventLoop::ExcludeUserInputEvents);
	return isConnected();
}

void ServerConnection::disconnectFromServer()
{
	if(!sock)return;
	QMetaObject::invokeMethod(sock,"disconnectFromServer",Qt::QueuedConnection);
}

bool ServerConnection::subscribeStorage(const QByteArray &devIdOrName,const QByteArray &sensorName)
{
	return execCommand("subscribe",QByteArrayList()<<devIdOrName<<sensorName);
}

bool ServerConnection::unsubscribeStorage(const QByteArray &devIdOrName,const QByteArray &sensorName)
{
	return execCommand("unsubscribe",QByteArrayList()<<devIdOrName<<sensorName);
}

bool ServerConnection::identifyServer(QUuid &id,QByteArray &name)
{
	QByteArrayList retVal;
	if(!execCommand(WLIOTProtocolDefs::identifyMsg,QByteArrayList(),retVal)||retVal.count()<2)
		return false;
	id=QUuid(retVal[0]);
	name=retVal[1];
	return !id.isNull();
}

bool ServerConnection::writeMsg(const Message &m)
{
	if(!sock)return false;
	QByteArray msgData=m.dump();
	QMetaObject::invokeMethod(sock,"writeData",Qt::QueuedConnection,Q_ARG(QByteArray,msgData));
	return true;
}

void ServerConnection::setProxy(const QNetworkProxy &p)
{
	proxy=p;
}

bool ServerConnection::writeVDevMsg(const QUuid &id, const Message &m)
{
	return writeMsg(Message(WLIOTServerProtocolDefs::vdevMsg,QByteArrayList()<<id.toByteArray()<<m.title<<m.args));
}

ApmIdType ServerConnection::userId()
{
	return uid;
}

void ServerConnection::onNetDeviceConnected()
{
	emit needAuthentication();
}

void ServerConnection::onDevDisconnected()
{
	if(!noDebug)qDebug()<<"iot server disconnected";
	emit disconnected();
	sock->deleteLater();
	sockThread->quit();
	sockThread->wait(3000);
	sockThread->terminate();
	parser.reset();
	sock=0;
	ready=false;
	uid=-1;
	syncTimer.stop();
}

void ServerConnection::onRawMessage(const Message &m)
{
	if(m.title==WLIOTProtocolDefs::devSyncMsg)
	{
//		qDebug()<<"iot sync";
		wasSyncMsg=true;
		writeMsg(Message(WLIOTProtocolDefs::devSyncrMsg));
	}
	else if(m.title==WLIOTProtocolDefs::measurementMsg)
	{
		if(m.args.count()<3)return;
		QUuid devId(m.args[0]);
		QByteArray sensorName=m.args[1];
		if(devId.isNull())return;
		StorageId stId={devId,sensorName};
		QMetaObject::invokeMethod(this,"newSensorValue",Qt::QueuedConnection,Q_ARG(WLIOT::StorageId,stId),
			Q_ARG(QByteArrayList,m.args.mid(2)));
	}
	else if(m.title==WLIOTServerProtocolDefs::notifyDeviceIdentifiedMsg)
	{
		if(m.args.count()<2)return;
		QUuid devId(m.args[0]);
		QByteArray devName=m.args[1];
		QUuid typeId;
		if(m.args.count()>2)
			typeId=QUuid(m.args[2]);
		if(devId.isNull())return;
		emit deviceIdentified(devId,devName,typeId);
	}
	else if(m.title==WLIOTServerProtocolDefs::notifyDeviceLostMsg)
	{
		if(m.args.count()<1)return;
		QUuid devId(m.args[0]);
		if(devId.isNull())return;
		emit deviceLost(devId);
	}
	else if(m.title==WLIOTProtocolDefs::stateChangedMsg)
	{
		if(m.args.count()<1)return;
		QUuid devId(m.args[0]);
		QByteArrayList args=m.args.mid(1);
		if(args.count()%3!=0)return;
		emit deviceStateChanged(devId,args);
	}
	else if(m.title==WLIOTServerProtocolDefs::notifyStorageCreatedMsg)
	{
		StorageDescr st;
		if(!StoragesCommands::storageFromArgs(m.args,st))return;
		emit storageCreated(st);
	}
	else if(m.title==WLIOTServerProtocolDefs::notifyStorageRemovedMsg)
	{
		if(m.args.count()<2)return;
		QUuid devId(m.args[0]);
		QByteArray sensorName=m.args[1];
		if(devId.isNull())return;
		emit storageRemoved({devId,sensorName});
	}
	else if(m.title==WLIOTServerProtocolDefs::vdevMsg)
	{
		if(m.args.count()<2)return;
		QUuid id(m.args[0]);
		if(id.isNull())return;
		emit vdevMsg(id,Message(m.args[1],m.args.mid(2)));
	}
	else if(m.title==WLIOTProtocolDefs::funcAnswerOkMsg||m.title==WLIOTProtocolDefs::funcAnswerErrMsg||
		m.title==WLIOTServerProtocolDefs::srvCmdDataMsg)
	{
		emit funcCallReplyMsg(m);
	}
}

void ServerConnection::onLocalSocketConnected()
{
	if(!noDebug)qDebug()<<"ServerConnection::onLocalSocketConnected";
	ready=true;
	emit preconnected();
	uid=0;
	emit connected();
	emit authenticationChanged();
	wasSyncMsg=true;
	syncTimer.start();
}

void ServerConnection::onSyncTimer()
{
//	qDebug()<<"iot connection onSyncTimer";
	if(wasSyncMsg)
		wasSyncMsg=false;
	else if(isConnected())
	{
		if(!noDebug)qDebug()<<"iot server sync timeout";
		disconnectFromServer();
	}
}

void ServerConnection::onNewData(QByteArray data)
{
	parser.pushData(data);
}
