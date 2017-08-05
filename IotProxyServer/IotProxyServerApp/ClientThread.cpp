#include "ClientThread.h"

ClientThread::ClientThread(QLocalSocket *sock,QObject *parent)
	:QThread(parent)
{
	socket=sock;
	socket->setParent(0);
	dev=new ARpcOutsideDevice(sock);
	proc=new IotProxyCommandProcessor(dev);
	start();
	connect(socket,&QLocalSocket::connected,dev,&ARpcOutsideDevice::onDeviceOpened,Qt::DirectConnection);
	connect(this,&ClientThread::started,this,&ClientThread::onStarted,Qt::DirectConnection);
}

ClientThread::~ClientThread()
{
	quit();
	wait();
	delete proc;
	delete dev;
	delete socket;
}

void ClientThread::onStarted()
{
	socket->moveToThread(this);
	dev->moveToThread(this);
	proc->moveToThread(this);
}
