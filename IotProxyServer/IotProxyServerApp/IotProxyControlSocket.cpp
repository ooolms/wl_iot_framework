#include "IotProxyControlSocket.h"
#include <QDebug>

const QString localServerName=QString("WLIotProxyServer");

IotProxyControlSocket::IotProxyControlSocket(QObject *parent)
	:QObject(parent)
{
	QLocalServer::removeServer(localServerName);
	localServer.listen(localServerName);
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
		ARpcOutsideDevice *dev=new ARpcOutsideDevice(sock,this);
		clientDevices.append(dev);
		clientCmdProcs.append(new IotProxyCommandProcessor(dev,this));
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
