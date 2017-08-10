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

#include "IotProxyControlSocket.h"
#include <QThread>
#include <QDebug>
#include <sys/stat.h>

const QString localServerName=QString("wliotproxyd");

IotProxyControlSocket::IotProxyControlSocket(QObject *parent)
	:QObject(parent)
{
	QLocalServer::removeServer(localServerName);
	auto msk=umask(0);
	localServer.listen(localServerName);
	umask(msk);
	connect(&localServer,&QLocalServer::newConnection,this,&IotProxyControlSocket::onNewLocalConnection);
}

IotProxyControlSocket::~IotProxyControlSocket()
{
	for(auto &set:clients)
	{
		set.sock->disconnect(this);
		set.thr->quit();
		set.thr->wait(1000);
		if(set.proc)delete set.proc;
		if(set.dev)delete set.dev;
		delete set.thr;
		delete set.sock;
	}
	clients.clear();
}

void IotProxyControlSocket::onNewLocalConnection()
{
	QLocalSocket *sock=localServer.nextPendingConnection();
	while(sock!=0)
	{
		qDebug()<<"Client connected";
		connect(sock,&QLocalSocket::disconnected,this,
			&IotProxyControlSocket::onLocalSocketDisconnected,Qt::QueuedConnection);
		sock->setParent(0);
		ClientSet set;
		set.sock=sock;
		QThread *thr=new QThread;
		thr->setObjectName("Client thread");
		set.thr=thr;
		connect(thr,&QThread::started,this,&IotProxyControlSocket::onThreadStarted);
		clients.append(set);
		thr->start();
		sock=localServer.nextPendingConnection();
	}
}

void IotProxyControlSocket::onLocalSocketDisconnected()
{
	int index=findClient((QLocalSocket*)sender());
	if(index==-1)return;
	ClientSet &set=clients[index];
	set.thr->quit();
	set.thr->wait(1000);
	if(set.proc)delete set.proc;
	if(set.dev)delete set.dev;
	delete set.thr;
	delete set.sock;
	clients.removeAt(index);
	qDebug()<<"Client disconnected";
}

void IotProxyControlSocket::onThreadStarted()
{
	int index=findClient((QThread*)sender());
	if(index==-1)return;
	ClientSet &set=clients[index];
	ARpcOutsideDevice *dev=new ARpcOutsideDevice(set.sock);
	connect(set.sock,&QLocalSocket::connected,dev,&ARpcOutsideDevice::onDeviceOpened,Qt::DirectConnection);
	IotProxyCommandProcessor *cProc=new IotProxyCommandProcessor(dev);
	set.sock->moveToThread(set.thr);
	dev->moveToThread(set.thr);
	cProc->moveToThread(set.thr);
	QMetaObject::invokeMethod(dev,"readReadyData",Qt::QueuedConnection);
	set.dev=dev;
	set.proc=cProc;
}

int IotProxyControlSocket::findClient(QLocalSocket *sock)
{
	for(int i=0;i<clients.count();++i)
		if(clients[i].sock==sock)return i;
	return -1;
}

int IotProxyControlSocket::findClient(QThread *thr)
{
	for(int i=0;i<clients.count();++i)
		if(clients[i].thr==thr)return i;
	return -1;
}
