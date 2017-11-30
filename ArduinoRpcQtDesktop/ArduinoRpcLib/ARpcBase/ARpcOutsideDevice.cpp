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

#include "ARpcOutsideDevice.h"
#include "ARpcBase/ARpcSyncCall.h"
#include <QTimer>
#include <QDebug>
#include <QEventLoop>

ARpcOutsideDevice::ARpcOutsideDevice(QIODevice *d,QObject *parent)
	:ARpcDevice(parent)
{
	dev=d;
	mIsConnected=dev->isOpen();
	if(dev)
	{
		connect(dev,&QIODevice::aboutToClose,this,&ARpcOutsideDevice::onDeviceDisconnected,Qt::DirectConnection);
		connect(dev,&QIODevice::destroyed,this,&ARpcOutsideDevice::onDeviceDestroyed,Qt::DirectConnection);
		connect(dev,&QIODevice::readyRead,this,&ARpcOutsideDevice::onDataReady,Qt::DirectConnection);
	}
}

bool ARpcOutsideDevice::writeMsg(const QString &msg)
{
	return writeMsg(ARpcMessage(msg));
}

bool ARpcOutsideDevice::writeMsg(const QString &msg,const QStringList &args)
{
	return writeMsg(ARpcMessage(msg,args));
}

bool ARpcOutsideDevice::writeMsg(const ARpcMessage &m)
{
	return dev->write(ARpcStreamParser::dump(m).toUtf8())!=-1;
}

bool ARpcOutsideDevice::isConnected()
{
	return mIsConnected;
}

void ARpcOutsideDevice::onDeviceConnected()
{
	if(!mIsConnected)
	{
		mIsConnected=true;
		emit connected();
	}
}

void ARpcOutsideDevice::readReadyData()
{
	onDataReady();
}

void ARpcOutsideDevice::onDeviceDestroyed()
{
	dev=0;
	if(mIsConnected)
	{
		mIsConnected=false;
		emit disconnected();
	}
}

void ARpcOutsideDevice::onDeviceDisconnected()
{
	if(mIsConnected)
	{
		mIsConnected=false;
		emit disconnected();
	}
}

void ARpcOutsideDevice::onDataReady()
{
	QByteArray data=dev->readAll();
	if(!data.isEmpty())streamParser.pushData(QString::fromUtf8(data));
}
