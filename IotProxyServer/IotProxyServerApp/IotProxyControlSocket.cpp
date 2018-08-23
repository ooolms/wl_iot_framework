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
#include "IotProxyInstance.h"
#include <QThread>
#include <QDebug>
#include <sys/stat.h>

const QString localServerName=QString("wliotproxyd");

IotProxyControlSocket::IotProxyControlSocket(QObject *parent)
	:QObject(parent)
{
	connect(&localServer,&QLocalServer::newConnection,this,&IotProxyControlSocket::onNewLocalConnection);
}

IotProxyControlSocket::~IotProxyControlSocket()
{
	for(auto &set:clients)
	{
		set.sock->disconnect(this);
		set.proc->scheduleDelete();

//		set->sock()->disconnect(this);
//		set->quit();
//		set->wait(2000);
//		set->terminate();
//		//TODO gracefull stop
//		delete set;
	}
	clients.clear();
	QLocalServer::removeServer(localServerName);
}

void IotProxyControlSocket::start()
{
	QLocalServer::removeServer(localServerName);
	auto msk=umask(000);
	localServer.listen(localServerName);
	umask(msk);
}

void IotProxyControlSocket::onNewLocalConnection()
{
	QLocalSocket *sock=localServer.nextPendingConnection();
	while(sock!=0)
	{
		//TODO multithread ??? "QSocketNotifier: Socket notifiers cannot be enabled or disabled from another thread"
		qDebug()<<"Client connected";
		connect(sock,&QLocalSocket::disconnected,this,
			&IotProxyControlSocket::onLocalSocketDisconnected,Qt::QueuedConnection);

		ARpcOutsideDevice *dev=new ARpcOutsideDevice(sock);
		sock->setParent(dev);
		IotProxyCommandProcessor *cProc=new IotProxyCommandProcessor(dev,false);
		ClientSet set;
		set.sock=sock;
		set.dev=dev;
		set.proc=cProc;
		clients.append(set);
		dev->readReadyData();

//		ClientThread *thr=new ClientThread(sock,this);
//		thr->setup();
//		clients.append(thr);

		sock=localServer.nextPendingConnection();
	}
}

void IotProxyControlSocket::onLocalSocketDisconnected()
{
	int index=findClient((QLocalSocket*)sender());
	if(index==-1)return;
	ClientSet &set=clients[index];
	set.proc->scheduleDelete();
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

int IotProxyControlSocket::findClient(QLocalSocket *sock)
{
	for(int i=0;i<clients.count();++i)
		if(clients[i].sock==sock)return i;
	return -1;
}
