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

#include "IotServerConnection.h"
#include "IotServerStoragesCommands.h"
#include "IotServerCommandCall.h"
#include <QDebug>
#include <QEventLoop>

const QString localServerName=QString("wliotproxyd");

IotServerConnection::IotServerConnection(QObject *parent)
	:QObject(parent)
{
	localSock=0;
	callIdNum=0;
	netAuthentificated=false;
	proxy=QNetworkProxy(QNetworkProxy::NoProxy);
	connect(&parser,&StreamParser::newMessage,this,&IotServerConnection::onRawMessage);
}

bool IotServerConnection::startConnectLocal()
{
	if(localSock)return false;
	netConn=false;
	parser.reset();
	localSock=new QLocalSocket(this);
	localSock->connectToServer(localServerName);
	connect(localSock,&QLocalSocket::connected,this,&IotServerConnection::connected);
	connect(localSock,&QLocalSocket::disconnected,this,&IotServerConnection::onDevDisconnected);
	connect(localSock,static_cast<void (QLocalSocket::*)(QLocalSocket::LocalSocketError)>(&QLocalSocket::error),
		this,&IotServerConnection::onLocalSocketError);
	connect(localSock,&QSslSocket::readyRead,this,&IotServerConnection::onLocalReadyRead);
	return true;
}

bool IotServerConnection::startConnectNet(const QString &host,quint16 port)
{
	if(netSock)return false;
	netConn=true;
	netAuthentificated=false;
	netSock=new QSslSocket(this);
	netSock->setProxy(proxy);
	netSock->setPeerVerifyMode(QSslSocket::VerifyNone);
	parser.reset();
	netSock->connectToHostEncrypted(host,port);
	connect(netSock,&QSslSocket::encrypted,this,&IotServerConnection::onNetDeviceConnected,Qt::QueuedConnection);
	connect(netSock,&QSslSocket::disconnected,this,&IotServerConnection::onDevDisconnected);
	connect(netSock,static_cast<void(QSslSocket::*)(QAbstractSocket::SocketError)>(&QSslSocket::error),
		this,&IotServerConnection::onNetError);
	connect(netSock,static_cast<void(QSslSocket::*)(const QList<QSslError>&)>(&QSslSocket::sslErrors),
		this,&IotServerConnection::onSslError);
	connect(netSock,&QSslSocket::disconnected,this,&IotServerConnection::onDevDisconnected);
	connect(netSock,&QSslSocket::readyRead,this,&IotServerConnection::onNetReadyRead);
	return true;
}

bool IotServerConnection::authentificateNet(const QByteArray &userName,const QByteArray &pass)
{
	if(!netSock->isEncrypted())
		return false;
	if(!execCommand(ServerConfig::authenticateSrvMsg,QByteArrayList()<<userName<<pass))return false;
	netAuthentificated=true;
	emit connected();
	return true;
}

bool IotServerConnection::authentificateLocalFromRoot(const QByteArray &userName)
{
	if(!localSock->isOpen())return false;
	return execCommand(ServerConfig::authenticateSrvMsg,QByteArrayList()<<userName);
}

bool IotServerConnection::isConnected()
{
	if(!netSock)return false;
	if(netConn)return netSock->isEncrypted()&&netAuthentificated;
	else return localSock->isOpen();
}

bool IotServerConnection::execCommand(const QByteArray &cmd,
	const QByteArrayList &args,QByteArrayList &retVal,CmDataCallback onCmData)
{
	IotServerCommandCall call(this,onCmData,cmd,args);
	retVal=call.returnValue();
	return call.ok();
}

bool IotServerConnection::execCommand(const QByteArray &cmd,const QByteArrayList &args,CmDataCallback onCmData)
{
	QByteArrayList retVal;
	return execCommand(cmd,args,retVal,onCmData);
}

bool IotServerConnection::waitForConnected(int msec)
{
	if(!netSock)return false;
	if(netConn)
		return netSock->waitForEncrypted(msec);
	else return localSock->waitForConnected(msec);
}

void IotServerConnection::disconnectFromServer()
{
	if(!netSock)return;
	if(netConn)
	{
		netSock->disconnectFromHost();
		netAuthentificated=false;
	}
	else localSock->disconnectFromServer();
}

bool IotServerConnection::subscribeStorage(const QByteArray &devIdOrName,const QByteArray &sensorName)
{
	return execCommand("subscribe",QByteArrayList()<<devIdOrName<<sensorName);
}

bool IotServerConnection::unsubscribeStorage(const QByteArray &devIdOrName,const QByteArray &sensorName)
{
	return execCommand("unsubscribe",QByteArrayList()<<devIdOrName<<sensorName);
}

bool IotServerConnection::identifyServer(QUuid &id,QByteArray &name)
{
	QByteArrayList retVal;
	if(!execCommand(WLIOTConfig::identifyMsg,QByteArrayList(),retVal)||retVal.count()<2)
		return false;
	id=QUuid(retVal[0]);
	name=retVal[1];
	return !id.isNull();
}

bool IotServerConnection::writeMsg(const Message &m)
{
	if(!localSock)return false;
	QByteArray msgData=m.dump();
	if(netConn)
	{
		if(netSock->write(msgData)!=msgData.size())return false;
		netSock->flush();
		return true;
	}
	else
	{
		if(localSock->write(msgData)!=msgData.size())return false;
		localSock->flush();
		return true;
	}
}

void IotServerConnection::setProxy(const QNetworkProxy &p)
{
	proxy=p;
	if(netConn&&netSock)
		netSock->setProxy(proxy);
}

void IotServerConnection::onNetDeviceConnected()
{
	emit needAuthentification();
}

void IotServerConnection::onLocalSocketError()
{
	qDebug()<<"iot server connection error: "<<localSock->errorString();
	localSock->deleteLater();
	localSock=0;
}

void IotServerConnection::onNetError()
{
	qDebug()<<"iot server connection error: "<<netSock->errorString();
	netSock->deleteLater();
	netSock=0;
}

void IotServerConnection::onSslError()
{
	qDebug()<<"iot server connection error: "<<netSock->sslErrors();
	/*delete dev;
	netSock->deleteLater();
	netSock=0;
	dev=0;*/
}

void IotServerConnection::onDevDisconnected()
{
	emit disconnected();
	parser.reset();
	if(netConn)
		delete netSock;
	else delete localSock;
	localSock=0;
	netAuthentificated=false;
}

void IotServerConnection::onRawMessage(const Message &m)
{
	if(m.title==WLIOTConfig::measurementMsg)
	{
		if(m.args.count()<3)return;
		QUuid devId(m.args[0]);
		QByteArray sensorName=m.args[1];
		if(devId.isNull())return;
		StorageId stId={devId,sensorName};
		QMetaObject::invokeMethod(this,"newSensorValue",Qt::QueuedConnection,Q_ARG(StorageId,stId),
			Q_ARG(QByteArrayList,m.args.mid(2)));
	}
	else if(m.title==ServerConfig::notifyDeviceIdentifiedMsg)
	{
		if(m.args.count()<3)return;
		QUuid devId(m.args[0]);
		QByteArray devName=m.args[1];
		if(devId.isNull())return;
		emit deviceIdentified(devId,devName,m.args[2]);
	}
	else if(m.title==ServerConfig::notifyDeviceLostMsg)
	{
		if(m.args.count()<1)return;
		QUuid devId(m.args[0]);
		if(devId.isNull())return;
		emit deviceLost(devId);
	}
	else if(m.title==WLIOTConfig::stateChangedMsg)
	{
		if(m.args.count()<1)return;
		QUuid devId(m.args[0]);
		QByteArrayList args=m.args.mid(1);
		if(args.count()%3!=0)return;
		emit deviceStateChanged(devId,args);
	}
	else if(m.title==ServerConfig::notifyStorageCreatedMsg)
	{
		IotServerStorageDescr st;
		if(!IotServerStoragesCommands::storageFromArgs(m.args,st))return;
		emit storageCreated(st);
	}
	else if(m.title==ServerConfig::notifyStorageRemovedMsg)
	{
		if(m.args.count()<2)return;
		QUuid devId(m.args[0]);
		QByteArray sensorName=m.args[1];
		if(devId.isNull())return;
		emit storageRemoved({devId,sensorName});
	}
	else if(m.title=="vdev_command")
	{
		if(m.args.count()<3)return;
		QByteArray callId=m.args[0];
		QUuid devId(m.args[1]);
		QByteArray cmd=m.args[2];
		QByteArrayList args=m.args.mid(3),retVal;
		bool ok=false;
		emit processVirtualDeviceCommand(devId,cmd,args,ok,retVal);
		writeMsg(Message(ok?"vdev_ok":"vdev_err",QByteArrayList()<<callId<<retVal));
	}
	else if(m.title==WLIOTConfig::funcAnswerOkMsg||m.title==WLIOTConfig::funcAnswerErrMsg||
		m.title==ServerConfig::srvCmdDataMsg)
	{
		emit funcCallReplyMsg(m);
	}
}

void IotServerConnection::onLocalReadyRead()
{
	QByteArray data=localSock->readAll();
//	qDebug()<<"RAW: "<<data;
	parser.pushData(data);
}

void IotServerConnection::onNetReadyRead()
{
	QByteArray data=netSock->readAll();
//	qDebug()<<"RAW: "<<data;
	parser.pushData(data);
}
