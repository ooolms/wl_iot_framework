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

#ifndef CLIENTTHREAD_H
#define CLIENTTHREAD_H

#include <QThread>
#include <QLocalSocket>
#include "wliot/devices/QtIODeviceWrap.h"
#include "CommandProcessor.h"

class ClientThread
	:public QThread
{
	Q_OBJECT
public:
	explicit ClientThread(QLocalSocket *s,bool needAuth,QObject *parent=0);
	virtual ~ClientThread();
	void setup();
	QLocalSocket* sock();

protected:
	virtual void run();

private:
	QLocalSocket *socket;
	QtIODeviceWrap *dev;
	CommandProcessor *proc;
	bool mNeedAuth;
};

#endif // CLIENTTHREAD_H
