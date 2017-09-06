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

#include "ClientThread.h"

ClientThread::ClientThread(QLocalSocket *s,QObject *parent)
	:QThread(parent)
{
	socket=s;
	socket->setParent(0);
	dev=0;
	proc=0;
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
