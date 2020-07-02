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

#include "wliot/devices/SerialDeviceBackend.h"
#include "SerialDriver.h"
#include "SerialNotificator.h"
#include <QFileInfo>
#include <QDebug>
#include <QSerialPortInfo>
#include <QThread>

using namespace WLIOT;

const QByteArray SerialDeviceBackend::mBackendType=QByteArray("tty");

SerialDeviceBackend::SerialDeviceBackend(const QString &portName,const QString &connOptions,QObject *parent)
	:ILowLevelDeviceBackend(portName,parent)
{
	ttyPort=new SerialDriver(portName,connOptions,this);

	connect(&SerialNotificator::inst(),&SerialNotificator::checkSerialPorts,
		this,&SerialDeviceBackend::onDevDirChanged);
	connect(ttyPort,SIGNAL(newData(QByteArray)),this,SLOT(onNewData(QByteArray)),Qt::QueuedConnection);
	connect(ttyPort,&SerialDriver::error,this,&SerialDeviceBackend::onPortError);

	tryOpen();
}

SerialDeviceBackend::~SerialDeviceBackend()
{
	closeTty();
}

void SerialDeviceBackend::onNewData(const QByteArray &data)
{
	if(!data.isEmpty())
		emit newData(data);
}

void SerialDeviceBackend::onDevDirChanged()
{
	if(ttyPort->isOpened()&&!QFile::exists("/dev/"+ttyPort->portName()))
		closeTty();
}

void SerialDeviceBackend::onPortError()
{
	if(ttyPort->errorCode()!=SerialDriver::NoError&&ttyPort->errorCode()!=SerialDriver::UnknownError)
	{
		qDebug()<<"Closing tty port";
		closeTty();
	}
}

void SerialDeviceBackend::tryOpen()
{
	if(isConnected())
		return;
	if(!ttyPort->open())
		return;
	ttyPort->startReader();
	emit connected();
}

void SerialDeviceBackend::closeTty()
{
	if(ttyPort->isOpened())
	{
		ttyPort->close();
		emit disconnected();
	}
}

bool SerialDeviceBackend::writeData(const QByteArray &data)
{
	if(!ttyPort->isOpened())return false;
	ttyPort->write(data);
	return true;
}

bool SerialDeviceBackend::flush()
{
	//no action
	return ttyPort->isOpened();
}

bool SerialDeviceBackend::isConnected()const
{
	return ttyPort->isOpened();
}

void SerialDeviceBackend::forceDisconnect()
{
	if(ttyPort->isOpened())
		ttyPort->close();
}

QByteArray SerialDeviceBackend::backendType()const
{
	return mBackendType;
}

bool WLIOT::SerialDeviceBackend::waitForConnected(int)
{
	return ttyPort->isOpened();
}
