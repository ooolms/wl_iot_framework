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
	:QObject(parent)
	,streamParser(&msgParser)
{
	dev=d;
	connect(&streamParser,&ARpcStreamParser::processMessage,this,&ARpcOutsideDevice::rawMessage);
	if(dev)
	{
		connect(dev,&QIODevice::aboutToClose,this,&ARpcOutsideDevice::disconnected);
		connect(dev,&QIODevice::destroyed,this,&ARpcOutsideDevice::onDeviceDestroyed);
		connect(dev,&QIODevice::readyRead,this,&ARpcOutsideDevice::onDataReady);
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
	if(!dev)return false;
	return dev->write((msgParser.dump(m)+ARpcConfig::msgDelim).toUtf8());
}

void ARpcOutsideDevice::onDeviceDestroyed()
{
	dev=0;
}

void ARpcOutsideDevice::onDataReady()
{
	QByteArray data=dev->readAll();
	streamParser.pushData(QString::fromUtf8(data));
}
