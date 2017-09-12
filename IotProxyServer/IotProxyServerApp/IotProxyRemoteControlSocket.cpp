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
#include <QThread>
#include <QDebug>
#include <sys/stat.h>

IotProxyRemoteControlSocket::IotProxyRemoteControlSocket(QObject *parent)
	:QObject(parent)
{
	sslServer.listen(QHostAddress::Any,ARpcConfig::netDeviceSslPort);
	connect(&sslServer,&QSslServer::newConnection,this,&IotProxyRemoteControlSocket::onNewLocalConnection);
}

IotProxyRemoteControlSocket::~IotProxyRemoteControlSocket()
{
	for(auto &set:clients)
	{
		set.sock->disconnect(this);
		if(set.proc)delete set.proc;
		if(set.dev)delete set.dev;
		delete set.sock;

//		set->sock()->disconnect(this);
//		set->quit();
//		set->wait(2000);
//		set->terminate();
//		//TODO gracefull stop
//		delete set;
	}
	clients.clear();
}

void IotProxyRemoteControlSocket::onNewLocalConnection()
{
	QSslSocket *sock=(QSslSocket*)sslServer.nextPendingConnection();
	while(sock!=0)
	{
		//TODO multithread ??? "QSocketNotifier: Socket notifiers cannot be enabled or disabled from another thread"
		qDebug()<<"Client connected";
		connect(sock,&QSslSocket::disconnected,this,
			&IotProxyRemoteControlSocket::onSocketDisconnected,Qt::QueuedConnection);

		connect(sock,&QSslSocket::encrypted,this,&IotProxyRemoteControlSocket::onSocketEncrypted);
		connect(sock,static_cast<void (QSslSocket::*)(const QList<QSslError>&)>(&QSslSocket::sslErrors),
			this,&IotProxyRemoteControlSocket::onSocketDisconnected);
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
	int index=findClient((QSslSocket*)sender());
	if(index==-1)return;
	ClientSet &set=clients[index];
	if(set.proc)delete set.proc;
	if(set.dev)delete set.dev;
	delete set.sock;
	clients.removeAt(index);
	qDebug()<<"Client disconnected";

//	ClientThread *thr=clients[index];
//	thr->quit();
//	thr->wait(2000);
//	thr->terminate();
//	delete thr;
//	clients.removeAt(index);
	//	qDebug()<<"Client disconnected";
}

void IotProxyRemoteControlSocket::onSocketEncrypted()
{
	int index=findClient((QSslSocket*)sender());
	if(index==-1)return;
	ClientSet &set=clients[index];
	ARpcOutsideDevice *dev=new ARpcOutsideDevice(set.sock);
	IotProxyCommandProcessor *cProc=new IotProxyCommandProcessor(dev);
	set.dev=dev;
	set.proc=cProc;
	dev->readReadyData();
}

int IotProxyRemoteControlSocket::findClient(QSslSocket *sock)
{
	for(int i=0;i<clients.count();++i)
		if(clients[i].sock==sock)return i;
	return -1;
}
