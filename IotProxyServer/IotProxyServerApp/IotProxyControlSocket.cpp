#include "IotProxyControlSocket.h"
#include <QThread>
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
		ARpcOutsideDevice *dev=new ARpcOutsideDevice(sock);
		IotProxyCommandProcessor *cProc=new IotProxyCommandProcessor(dev);
		QThread *thr=new QThread;
		clientDevices.append(dev);
		clientCmdProcs.append(cProc);
		clientThreads.append(thr);
		thr->start();
		bool thrStarted=false;
		auto conn=connect(thr,&QThread::started,[thr,dev,cProc,sock,&thrStarted]()
		{
			sock->moveToThread(thr);
			dev->moveToThread(thr);
			cProc->moveToThread(thr);
			thrStarted=true;
		});
		while(!thrStarted)
			QThread::yieldCurrentThread();
		QObject::disconnect(conn);
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
