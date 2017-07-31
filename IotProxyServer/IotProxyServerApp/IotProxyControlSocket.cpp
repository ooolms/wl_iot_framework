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
}

void IotProxyControlSocket::onNewLocalConnection()
{
	QLocalSocket *sock=localServer.nextPendingConnection();
	while(sock!=0)
	{
		connect(sock,&QLocalSocket::disconnected,this,&IotProxyControlSocket::onLocalSocketDisconnected);
		localClients.append(sock);
		ARpcOutsideDevice *dev=new ARpcOutsideDevice(sock);
		IotProxyCommandProcessor *cProc=new IotProxyCommandProcessor(dev);
		clientDevices.append(dev);
		clientCmdProcs.append(cProc);
		sock=localServer.nextPendingConnection();
	}
}

void IotProxyControlSocket::onLocalSocketDisconnected()
{
	int index=localClients.indexOf((QLocalSocket*)sender());
	if(index==-1)return;
	delete clientCmdProcs[index];
	delete clientDevices[index];
	localClients[index]->deleteLater();
	localClients.removeAt(index);
	clientCmdProcs.removeAt(index);
	clientDevices.removeAt(index);
}
