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

#include "FakeDevice.h"
#include "wliot/WLIOTProtocolDefs.h"
#include <QDebug>
#include <QThread>

FakeDeviceBackend::FakeDeviceBackend(IFakeDeviceCallback *cb,QObject *parent)
	:RealDevice(parent)
{
	devId=QUuid::createUuid();
	cmdCb=cb;
	proc=new FakeDeviceMessageProc(this);
	cmdCb->setParent(proc);

	connect(cmdCb,&IFakeDeviceCallback::newMessageFromDevice,proc,&FakeDeviceMessageProc::newMessageFromDevice,
		Qt::DirectConnection);
	connect(cmdCb,&IFakeDeviceCallback::devIsReset,this,&FakeDeviceBackend::resetDevice,
		Qt::DirectConnection);
	connect(proc,&FakeDeviceMessageProc::newMessageFromDevice,
		this,&FakeDeviceBackend::onNewMessage,Qt::QueuedConnection);

	procThrd.start();
	while(!procThrd.isRunning())
		QThread::yieldCurrentThread();
	proc->moveToThread(&procThrd);
	onConnected();
}

FakeDeviceBackend::~FakeDeviceBackend()
{
	procThrd.quit();
	procThrd.wait(3000);
	procThrd.terminate();
	proc->moveToThread(QThread::currentThread());
	delete proc;
}

void FakeDeviceBackend::setConnected(bool c)
{
	if(c)
		onConnected();
	else
	{
		proc->resetDevice();
		onDisconnected();
	}
}

void FakeDeviceBackend::resetDevice()
{
	proc->resetDevice();
	onDeviceReset();
}

bool FakeDeviceBackend::writeMsgToDevice(const Message &m)
{
	if(!isConnected())return false;
	proc->writeMessageToDevice(m);
	return true;
}

FakeDeviceMessageProc::FakeDeviceMessageProc(FakeDeviceBackend *d)
{
	dev=d;
}

void FakeDeviceMessageProc::writeMessageToDevice(const Message &m)
{
	QMutexLocker lock(&msgQueueMt);
	msgQueue.append(m);
	if(msgQueue.count()==1)
		QMetaObject::invokeMethod(this,"processMessagesToDevice",Qt::QueuedConnection);
}

void FakeDeviceMessageProc::resetDevice()
{
	QMutexLocker lock(&msgQueueMt);
	msgQueue.clear();
}

void FakeDeviceMessageProc::processMessagesToDevice()
{
	Message m;
	while(1)
	{
		{
			QMutexLocker lock(&msgQueueMt);
			if(msgQueue.isEmpty())return;
			m=msgQueue.takeFirst();
		}
		processMessageToDevice(m);
	}
}

void FakeDeviceMessageProc::processMessageToDevice(const Message &m)
{
	if(m.title==WLIOTProtocolDefs::identifyMsg)
	{
		qDebug()<<"Identify request";
		emit newMessageFromDevice(Message(WLIOTProtocolDefs::deviceInfoMsg,
			QByteArrayList()<<dev->devId.toByteArray()<<"Test device"));
	}
	else if(m.title==WLIOTProtocolDefs::funcCallMsg&&m.args.count()>=2)
	{
		QByteArray callId=m.args[0];
		QByteArray cmd=m.args[1];
		QByteArrayList args=m.args.mid(2);
		qDebug()<<"Function called: "<<cmd<<"; args: "<<args.join("|");
		bool ok=false;
		QByteArrayList retVal;
		if(dev->cmdCb)
			ok=dev->cmdCb->processCommand(callId,cmd,args,retVal);
		if(ok)
			emit newMessageFromDevice(Message(WLIOTProtocolDefs::funcAnswerOkMsg,QByteArrayList()<<callId<<retVal));
		else emit newMessageFromDevice(Message(WLIOTProtocolDefs::funcAnswerErrMsg,QByteArrayList()<<callId<<retVal));
	}
	else if(m.title==WLIOTProtocolDefs::devSyncMsg)
	{
		if(dev->cmdCb)
			dev->cmdCb->onSyncMsg();
		else emit newMessageFromDevice(WLIOTProtocolDefs::devSyncrMsg);
	}
}

void IFakeDeviceCallback::onSyncMsg()
{
	emit newMessageFromDevice(WLIOTProtocolDefs::devSyncrMsg);
}
