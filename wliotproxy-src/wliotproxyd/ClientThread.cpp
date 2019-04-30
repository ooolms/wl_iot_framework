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

ClientThread::ClientThread(QLocalSocket *s,bool needAuth,QObject *parent)
	:QThread(parent)
{
	mNeedAuth=needAuth;
	socket=s;
	socket->setParent(0);
	dev=0;
	proc=0;
}

ClientThread::~ClientThread()
{
	socket->disconnectFromServer();
	socket->deleteLater();
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
	dev=new QtIODeviceWrap(socket);
	proc=new CommandProcessor(dev,mNeedAuth);
	dev->readReadyData();
	QThread::run();
	proc->disconnect(dev);
	proc->deleteLater();
	dev->deleteLater();
}

QLocalSocket* ClientThread::sock()
{
	return socket;
}
