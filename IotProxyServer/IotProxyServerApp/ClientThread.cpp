#include "ClientThread.h"

ClientThread::ClientThread(QLocalSocket *s,QObject *parent)
	:QThread(parent)
{
	socket=s;
	socket->setParent(0);
}

ClientThread::~ClientThread()
{
	socket->disconnectFromServer();
	delete socket;
}

void ClientThread::setup()
{
	start();
	while(!isRunning())
		QThread::yieldCurrentThread();
	socket->moveToThread(this);
}

void ClientThread::run()
{
	dev=new ARpcOutsideDevice(socket);
	proc=new IotProxyCommandProcessor(dev);
	dev->readReadyData();
	QThread::run();
	delete proc;
	delete dev;
}


QLocalSocket* ClientThread::sock()
{
	return socket;
}
