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

#ifndef FAKEDEVICEBACKEND_H
#define FAKEDEVICEBACKEND_H

#include "wliot/devices/RealDevice.h"
#include <QMutex>
#include <QThread>

class FakeDeviceBackend;

class IFakeDeviceCallback
	:public QObject
{
	Q_OBJECT
public:
	virtual bool processCommand(const QByteArray &callId,const QByteArray &cmd,
		const QByteArrayList &args,QByteArrayList &retVal)=0;
	virtual void onSyncMsg();

signals:
	void newMessageFromDevice(const Message &m);
	void devIsReset();
};

class FakeDeviceMessageProc
	:public QObject
{
	Q_OBJECT
public:
	explicit FakeDeviceMessageProc(FakeDeviceBackend *d);
	void writeMessageToDevice(const Message &m);
	void resetDevice();

signals:
	void newMessageFromDevice(Message m);

private:
	Q_INVOKABLE void processMessagesToDevice();
	void processMessageToDevice(const Message &m);

private:
	QList<Message> msgQueue;
	QMutex msgQueueMt;
	FakeDeviceBackend *dev;
};

class FakeDeviceBackend
	:public IHighLevelDeviceBackend
{
	Q_OBJECT
public:

public:
	explicit FakeDeviceBackend(IFakeDeviceCallback *cb,QObject *parent=0);
	virtual ~FakeDeviceBackend();
	virtual bool writeMessageToDevice(const Message &m)override;
	virtual bool isConnected()const override;
	virtual void forceDisconnect()override;
	virtual QByteArray type()const override;
	virtual QByteArray portOrAddress()const override;
	void setConnected(bool c);

public slots:
	void resetDevice();

public:

private:
	friend class FakeDeviceMessageProc;
	FakeDeviceMessageProc *proc;
	QThread procThrd;
	QUuid devId;
	IFakeDeviceCallback *cmdCb;
	bool mConnected;
};

#endif // FAKEDEVICEBACKEND_H
