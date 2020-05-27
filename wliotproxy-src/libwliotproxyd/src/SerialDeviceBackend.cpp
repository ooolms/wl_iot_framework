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

const QByteArray SerialDeviceBackend::devType=QByteArray("tty");

SerialDeviceBackend::SerialDeviceBackend(const QString &portName,QObject *parent)
	:ILowLevelDeviceBackend(parent)
{
	ttyPort=new SerialDriver(portName,this);

	connect(&SerialNotificator::inst(),&SerialNotificator::checkSerialPorts,
		this,&SerialDeviceBackend::onDevDirChanged);
	connect(ttyPort,SIGNAL(newData(QByteArray)),this,SLOT(onNewData(QByteArray)),Qt::QueuedConnection);
	connect(ttyPort,&SerialDriver::error,this,&SerialDeviceBackend::onPortError);
}

SerialDeviceBackend::~SerialDeviceBackend()
{
	closeTty();
}

QString SerialDeviceBackend::portName()const
{
	return ttyPort->portName();
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

//void ARpcTtyDevice::setBaudRate(qint32 rate,QSerialPort::Directions directions)
//{
//	ttyPort->setBaudRate(rate,directions);
//}

//void ARpcTtyDevice::setDataBits(QSerialPort::DataBits bits)
//{
//	ttyPort->setDataBits(bits);
//}

//void ARpcTtyDevice::setFlowControl(QSerialPort::FlowControl ctl)
//{
//	ttyPort->setFlowControl(ctl);
//}

//void ARpcTtyDevice::setParity(QSerialPort::Parity parity)
//{
//	ttyPort->setParity(parity);
//}

//void ARpcTtyDevice::setStopBits(QSerialPort::StopBits bits)
//{
//	ttyPort->setStopBits(bits);
//}

//qint32 ARpcTtyDevice::baudRate()
//{
//	return ttyPort->baudRate();
//}

//QSerialPort::DataBits ARpcTtyDevice::dataBits()
//{
//	return ttyPort->dataBits();
//}

//QSerialPort::FlowControl ARpcTtyDevice::flowControl()
//{
//	return ttyPort->flowControl();
//}

//QSerialPort::Parity ARpcTtyDevice::parity()
//{
//	return ttyPort->parity();
//}

//QSerialPort::StopBits ARpcTtyDevice::stopBits()
//{
//	return ttyPort->stopBits();
//}

void SerialDeviceBackend::closeTty()
{
	if(ttyPort->isOpened())
	{
		ttyPort->close();
		emit disconnected();
	}
}

void SerialDeviceBackend::setBaudRate(quint32 r)
{
	ttyPort->setBaudRate(r);
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

QByteArray SerialDeviceBackend::type()const
{
	return devType;
}

QByteArray SerialDeviceBackend::portOrAddress()const
{
	return ttyPort->portName().toUtf8();
}

//void ARpcTtyDevice::setupSerialPort()
//{
//	//терминальная магия
//	termios t;
//	msleep(100*1000);
//	if(tcgetattr(fd,&t))return;//ниасилил терминальную магию
//	msleep(100*1000);
//	cfsetspeed(&t,B9600);
//	t.c_iflag=0;
//	t.c_oflag=0;
//	t.c_cflag=CS8|B9600|CREAD|CLOCAL|HUPCL;
//	t.c_lflag=0;
//	t.c_line=0;
//	tcsetattr(fd,TCSANOW,&t);

//	ttyPort->setBaudRate(QSerialPort::Baud9600);
//	ttyPort->setDataBits(QSerialPort::Data8);
//	ttyPort->setFlowControl(QSerialPort::NoFlowControl);
//	ttyPort->setParity(QSerialPort::NoParity);
//	ttyPort->setStopBits(QSerialPort::OneStop);
//	ttyPort->setDataTerminalReady(true);
//}
