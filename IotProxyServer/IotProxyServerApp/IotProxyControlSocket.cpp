#include "IotProxyControlSocket.h"
#include <QThread>
#include <QDebug>
#include <sys/stat.h>

const QString localServerName=QString("WLIotProxyServer");

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
