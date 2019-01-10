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

#include "IotProxyRemoteControlSocket.h"
#include "ARpcBase/ARpcConfig.h"
#include "ARpcBase/ARpcServerConfig.h"
#include <QThread>
#include <QDebug>
#include <sys/stat.h>

IotProxyRemoteControlSocket::IotProxyRemoteControlSocket(QObject *parent)
	:QObject(parent)
{
	connect(&sslServer,&QSslServer::newConnection,this,&IotProxyRemoteControlSocket::onNewLocalConnection);
}

IotProxyRemoteControlSocket::~IotProxyRemoteControlSocket()
{
	for(auto &set:clients)
	{
		set.sock->disconnect(this);
		if(set.proc)
			set.proc->scheduleDelete();
		else delete set.sock;

		//		set->sock()->disconnect(this);
		//		set->quit();
		//		set->wait(2000);
		//		set->terminate();
		//		//TODO gracefull stop
		//		delete set;
	}
	clients.clear();
}

void IotProxyRemoteControlSocket::start(const QSslCertificate &crt,const QSslKey &key)
{
	if(sslServer.isListening())return;
	sslServer.setSslOptions(crt,key);
	sslServer.listen(QHostAddress::AnyIPv4,ARpcServerConfig::controlSslPort);
}

void IotProxyRemoteControlSocket::stop()
{
	sslServer.close();
}

void IotProxyRemoteControlSocket::onNewLocalConnection()
{
	QSslSocket *sock=(QSslSocket*)sslServer.nextPendingConnection();
	while(sock!=0)
	{
		//TODO multithread ??? "QSocketNotifier: Socket notifiers cannot be enabled or disabled from another thread"
		qDebug()<<"Client connected";
		connect(sock,&QSslSocket::disconnected,this,&IotProxyRemoteControlSocket::onSocketDisconnected,
			Qt::QueuedConnection);
		connect(sock,&QSslSocket::encrypted,this,&IotProxyRemoteControlSocket::onSocketEncrypted);
		connect(sock,static_cast<void (QSslSocket::*)(
			const QList<QSslError>&)>(&QSslSocket::sslErrors),this,
			&IotProxyRemoteControlSocket::onSslErrors);
		connect(sock,static_cast<void (QSslSocket::*)(
			QAbstractSocket::SocketError)>(&QSslSocket::error),this,
			&IotProxyRemoteControlSocket::onSocketError);
		ClientSet set;
		set.sock=sock;
		clients.append(set);

		//		ClientThread *thr=new ClientThread(sock,this);
		//		thr->setup();
		//		clients.append(thr);

		sock=(QSslSocket*)sslServer.nextPendingConnection();
	}
}

void IotProxyRemoteControlSocket::onSocketDisconnected()
{
	closeClient((QSslSocket*)sender());
	//	ClientThread *thr=clients[index];
	//	thr->quit();
	//	thr->wait(2000);
	//	thr->terminate();
	//	delete thr;
	//	clients.removeAt(index);ARpcConfig
	//	qDebug()<<"Client disconnected";
}

void IotProxyRemoteControlSocket::onSocketEncrypted()
{
	QSslSocket *sock=(QSslSocket*)sender();
	int index=findClient(sock);
	if(index==-1)
		return;
	ClientSet &set=clients[index];
	ARpcOutsideDevice *dev=new ARpcOutsideDevice(set.sock,[sock]()
	{
		sock->flush();
	});
	set.sock->setParent(dev);
	IotProxyCommandProcessor *cProc=new IotProxyCommandProcessor(dev,true);
	set.dev=dev;
	set.proc=cProc;
	dev->readReadyData();
}

void IotProxyRemoteControlSocket::onSocketError(QAbstractSocket::SocketError)
{
	QSslSocket *s=(QSslSocket*)sender();
	if(s->error()!=QSslSocket::RemoteHostClosedError)
		qWarning()<<"socket error: "<<s->errorString();
	s->disconnectFromHost();
}

void IotProxyRemoteControlSocket::onSslErrors(const QList<QSslError> &errs)
{
	for(const auto &e:errs)
		qWarning()<<"socket ssl error: "<<e.errorString()<<"\n";
}

int IotProxyRemoteControlSocket::findClient(QSslSocket *sock)
{
	for(int i=0;i<clients.count();++i)
		if(clients[i].sock==sock)
			return i;
	return -1;
}

void IotProxyRemoteControlSocket::closeClient(QSslSocket *sock)
{
	int index=findClient(sock);
	if(index==-1)
		return;
	ClientSet &set=clients[index];
	if(set.proc)
		set.proc->scheduleDelete();
	else delete set.sock;
	clients.removeAt(index);
	qDebug()<<"Client disconnected";
}
