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

#include "wliot/devices/QtIODeviceWrap.h"
#include "wliot/devices/CommandCall.h"
#include <QTimer>
#include <QDebug>
#include <QEventLoop>

QtIODeviceWrap::QtIODeviceWrap(QIODevice *d,OnDataWritten onDataWrittenFunc,QObject *parent)
	:QObject(parent)
{
	dev=d;
	mOnDataWritten=onDataWrittenFunc;
	if(dev)
	{
		connect(dev,&QIODevice::destroyed,this,&QtIODeviceWrap::onDeviceDestroyed,Qt::DirectConnection);
		connect(dev,&QIODevice::readyRead,this,&QtIODeviceWrap::onDataReady,Qt::DirectConnection);
	}
}

bool QtIODeviceWrap::writeMsg(const Message &m)
{
	QByteArray d=m.dump();
	return writeData(d);
}

bool QtIODeviceWrap::writeMsg(const QByteArray &m)
{
	QByteArray d=Message::dump(m);
	return writeData(d);
}

bool QtIODeviceWrap::writeMsg(const QByteArray &m,const QByteArrayList &args)
{
	QByteArray d=Message::dump(m,args);
	return writeData(d);
}

bool QtIODeviceWrap::isConnected()
{
	return mIsConnected;
}

QIODevice* QtIODeviceWrap::ioDev()
{
	return dev;
}

void QtIODeviceWrap::onDeviceConnected()
{
	if(!mIsConnected)
	{
		mIsConnected=true;
		emit connected();
	}
}

void QtIODeviceWrap::readReadyData()
{
	onDataReady();
}

void QtIODeviceWrap::onDeviceDestroyed()
{
	dev=0;
	if(mIsConnected)
	{
		mIsConnected=false;
		emit disconnected();
		streamParser.reset();
	}
}

void QtIODeviceWrap::onDeviceDisconnected()
{
	if(mIsConnected)
	{
		mIsConnected=false;
		emit disconnected();
		streamParser.reset();
	}
}

void QtIODeviceWrap::onDataReady()
{
	QByteArray data=dev->readAll();
	if(!data.isEmpty())
		streamParser.pushData(data);
}

bool QtIODeviceWrap::writeData(const QByteArray &data)
{
	qint64 bytes=dev->write(data);
	if(mOnDataWritten)
		mOnDataWritten();
	return bytes==data.size();
}
