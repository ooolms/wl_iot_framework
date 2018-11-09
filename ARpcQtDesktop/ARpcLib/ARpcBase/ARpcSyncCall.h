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

#ifndef ARPCSYNCCALL_H
#define ARPCSYNCCALL_H

#include "ARpcBase/ARpcConfig.h"
#include "ARpcBase/ARpcMessage.h"
#include <QObject>
#include <QEventLoop>
#include <QTimer>

class ARpcRealDevice;

class ARpcSyncCall
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcSyncCall(ARpcRealDevice *d,const QByteArray &func,QObject *parent=0);
	void setArgs(const QByteArrayList &args);
	void setUseCallMsg(bool u);
	void setTimeout(int msec);
	const QByteArrayList& returnValue();
	bool call();
	void abort();

private slots:
	void onNewMessage(const ARpcMessage &m);
	void onTimeout();
	void onDeviceDisconnected();
	void onDeviceDestroyed();
	void onStreamReset();

private:
	ARpcRealDevice *dev;
	QByteArrayList retVal;
	bool ok;
	enum
	{
		WAIT,
		EXEC,
		DONE
	}state;
	QEventLoop loop;
	QTimer timer;
	QByteArray callId;
	QByteArray mFunc;
	QByteArrayList mArgs;
	bool mUseCallMsg;
};

#endif // ARPCSYNCCALL_H
