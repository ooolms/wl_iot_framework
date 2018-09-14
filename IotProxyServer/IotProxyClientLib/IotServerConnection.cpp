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
#include <QDebug>
#include <QEventLoop>

const QString localServerName=QString("wliotproxyd");

IotServerConnection::IotServerConnection(QObject *parent)
	:QObject(parent)
{
	localSock=0;
	dev=0;
	callIdNum=0;
}

bool IotServerConnection::startConnectLocal()
{
	if(dev)return false;
	netConn=false;
	localSock=new QLocalSocket(this);
	dev=new ARpcOutsideDevice(localSock,this);
	localSock->connectToServer(localServerName);
	connect(localSock,&QLocalSocket::connected,dev,&ARpcOutsideDevice::onDeviceConnected);
	connect(localSock,&QLocalSocket::disconnected,dev,&ARpcOutsideDevice::onDeviceDisconnected);
	connect(localSock,static_cast<void (QLocalSocket::*)(QLocalSocket::LocalSocketError)>(&QLocalSocket::error),
		this,&IotServerConnection::onLocalSocketError);
	connect(dev,&ARpcOutsideDevice::connected,this,&IotServerConnection::connected);
	connect(dev,&ARpcOutsideDevice::disconnected,this,&IotServerConnection::onDevDisconnected);
	return true;
}

bool IotServerConnection::startConnectNet(const QString &host,quint16 port)
{
	if(dev)return false;
	netConn=true;
	netSock=new QSslSocket(this);
	netSock->setPeerVerifyMode(QSslSocket::VerifyNone);
	dev=new ARpcOutsideDevice(netSock,this);
	netSock->connectToHostEncrypted(host,port);
	connect(netSock,&QSslSocket::encrypted,this,&IotServerConnection::onNetDeviceConnected,Qt::QueuedConnection);
	connect(netSock,&QSslSocket::disconnected,dev,&ARpcOutsideDevice::onDeviceDisconnected);
	connect(netSock,static_cast<void(QSslSocket::*)(QAbstractSocket::SocketError)>(&QSslSocket::error),
		this,&IotServerConnection::onNetError);
	connect(netSock,static_cast<void(QSslSocket::*)(const QList<QSslError>&)>(&QSslSocket::sslErrors),
		this,&IotServerConnection::onSslError);
	connect(dev,&ARpcOutsideDevice::connected,this,&IotServerConnection::connected);
	connect(dev,&ARpcOutsideDevice::disconnected,this,&IotServerConnection::onDevDisconnected);
	connect(dev,&ARpcOutsideDevice::rawMessage,this,&IotServerConnection::onRawMessage);
	return true;
}

bool IotServerConnection::authentificateNet(const QByteArray &token)
{
	if(!netSock->isEncrypted())
		return false;
	if(!execCommand(ARpcServerConfig::authentificateSrvMsg,QByteArrayList()<<token))
		return false;
	dev->onDeviceConnected();
	return true;
}

bool IotServerConnection::isConnected()
{
	if(!dev)return false;
	return dev->isConnected();
}

bool IotServerConnection::execCommand(const QByteArray &cmd,
	const QByteArrayList &args,QByteArrayList &retVal,CmDataCallback onCmData)
{
	QEventLoop loop;
	QByteArray callId=QByteArray::number(++callIdNum);
	QTimer tmr;
	tmr.setSingleShot(true);
	tmr.setInterval(5000);
	connect(&tmr,&QTimer::timeout,&loop,&QEventLoop::quit);
	connect(dev,&ARpcOutsideDevice::disconnected,&loop,&QEventLoop::quit);
	bool done=false;
	auto conn=connect(dev,&ARpcOutsideDevice::rawMessage,[this,&loop,&done,&retVal,&callId,&onCmData,&tmr]
		(const ARpcMessage &m)
	{
		if(m.title==ARpcConfig::funcAnswerOkMsg&&!m.args.isEmpty()&&m.args[0]==callId)
		{
			done=true;
			retVal=m.args.mid(1);
			tmr.stop();
			loop.quit();
		}
		else if(m.title==ARpcConfig::funcAnswerErrMsg&&!m.args.isEmpty()&&m.args[0]==callId)
		{
			retVal=m.args.mid(1);
			tmr.stop();
			loop.quit();
		}
		else if(m.title==ARpcServerConfig::srvCmdDataMsg&&!m.args.isEmpty()&&m.args[0]==callId&&onCmData)
		{
			if(!onCmData(m.args.mid(1)))
			{
				tmr.stop();
				loop.quit();
			}
		}
	});
	connect(this,&IotServerConnection::disconnected,&loop,&QEventLoop::quit);
	connect(&tmr,&QTimer::timeout,&loop,&QEventLoop::quit);
	dev->writeMsg(cmd,QByteArrayList()<<callId<<args);
	tmr.start();
	loop.exec();
	disconnect(conn);
	return done;
}

bool IotServerConnection::execCommand(const QByteArray &cmd,
	const QByteArrayList &args,IotServerConnection::CmDataCallback onCmData)
{
	QByteArrayList retVal;
	return execCommand(cmd,args,retVal,onCmData);
}

bool IotServerConnection::waitForConnected(int msec)
{
	if(!dev)return false;
	if(netConn)
		return netSock->waitForEncrypted(msec);
	else return localSock->waitForConnected(msec);
}

void IotServerConnection::disconnectFromServer()
{
	if(!dev)return;
	if(netConn)
		netSock->disconnectFromHost();
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
	if(!execCommand(ARpcConfig::identifyMsg,QByteArrayList(),retVal)||retVal.count()<2)
		return false;
	id=QUuid(retVal[0]);
	name=retVal[1];
	return !id.isNull();
}

void IotServerConnection::onNetDeviceConnected()
{
	emit needAuthentification();
}

void IotServerConnection::onLocalSocketError()
{
	qDebug()<<"iot server connection error: "<<localSock->errorString();
	delete dev;
	localSock->deleteLater();
	localSock=0;
	dev=0;
}

void IotServerConnection::onNetError()
{
	qDebug()<<"iot server connection error: "<<netSock->errorString();
	delete dev;
	netSock->deleteLater();
	netSock=0;
	dev=0;
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
	delete dev;
	if(netConn)
		delete netSock;
	else delete localSock;
	localSock=0;
	dev=0;
}

void IotServerConnection::onRawMessage(const ARpcMessage &m)
{
	if(m.title==ARpcConfig::measurementMsg)
	{
		if(m.args.count()<3)return;
		QUuid devId(m.args[0]);
		QByteArray sensorName=m.args[1];
		if(devId.isNull())return;
		emit newSensorValue({devId,sensorName},m.args.mid(2));
	}
	else if(m.title==ARpcServerConfig::notifyDeviceIdentifiedMsg)
	{
		if(m.args.count()<2)return;
		QUuid devId(m.args[0]);
		QByteArray devName=m.args[1];
		if(devId.isNull())return;
		emit deviceIdentified(devId,devName);
	}
	else if(m.title==ARpcServerConfig::notifyDeviceLostMsg)
	{
		if(m.args.count()<1)return;
		QUuid devId(m.args[0]);
		if(devId.isNull())return;
		emit deviceLost(devId);
	}
	else if(m.title==ARpcServerConfig::notifyStorageCreatedMsg)
	{
		IotServerStorageDescr st;
		if(!IotServerStoragesCommands::storageFromArgs(m.args,st))return;
		emit storageCreated(st);
	}
	else if(m.title==ARpcServerConfig::notifyStorageRemovedMsg)
	{
		if(m.args.count()<2)return;
		QUuid devId(m.args[0]);
		QByteArray sensorName=m.args[1];
		if(devId.isNull())return;
		emit storageRemoved({devId,sensorName});
	}
	else if(m.title=="vdev_command")
	{
		if(m.args.count()<2)return;
		QByteArray callId=m.args[0];
		QByteArray cmd=m.args[1];
		QByteArrayList args=m.args.mid(2),retVal;
		bool ok=false;
		emit processVirtualDeviceCommand(cmd,args,ok,retVal);
		dev->writeMsg(ok?"vdev_ok":"vdev_err",QByteArrayList()<<callId<<retVal);
	}
}
