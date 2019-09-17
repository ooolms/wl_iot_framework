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

#include "wliot/devices/SerialDevice.h"
#include "SerialDriver.h"
#include "SerialNotificator.h"
#include <QFileInfo>
#include <QDebug>
#include <QSerialPortInfo>
#include <QThread>

SerialDevice::SerialDevice(const QString &portName,QObject *parent)
	:RealDevice(parent)
{
//	info=QSerialPortInfo(portName);
//	ttyPort=new QSerialPort(info,this);
//	ttyPort->setReadBufferSize(2000000);
//	connect(ttyPort,&QSerialPort::readyRead,this,&ARpcTtyDevice::onReadyRead);
//	connect(ttyPort,static_cast<void (QSerialPort::*)(
//		QSerialPort::SerialPortError)>(&QSerialPort::error),this,
//		&ARpcTtyDevice::onPortError);
//	file=new QFile(this);
//	fd=open(("/dev/"+portName.toUtf8()).constData(),O_RDWR|O_NOCTTY|O_SYNC);
//	if(fd!=-1)
//	{
//		notif=new QSocketNotifier(fd,QSocketNotifier::Read,this);
//		connect(notif,&QSocketNotifier::activated,this,&ARpcTtyDevice::onReadyRead);
//		file->open(fd,QIODevice::ReadWrite|QIODevice::Unbuffered);
//	}
//	else notif=0;
	ttyPort=new SerialDriver(portName,this);

	connect(&SerialNotificator::inst(),&SerialNotificator::checkSerialPorts,
		this,&SerialDevice::onDevDirChanged);
	connect(ttyPort,&SerialDriver::newData,this,&SerialDevice::onNewData,Qt::QueuedConnection);
	connect(ttyPort,&SerialDriver::error,this,&SerialDevice::onPortError);
	connect(&streamParser,&StreamParser::newMessage,this,&SerialDevice::onNewMessage);

//	tryOpen();
}

SerialDevice::~SerialDevice()
{
	closeTty();
}

bool SerialDevice::writeMsgToDevice(const Message &m)
{
	if(!isConnected())
		return false;
//	struct stat s;
//	if(fd==-1||fstat(fd,&s)!=0)
//	{
//		return false;
//	}
	QByteArray data=m.dump();
//	ttyPort->setRequestToSend(true);
//	if(ttyPort->write(data)!=data.size())return false;
//	if(!ttyPort->flush())return false;
//	ttyPort->setRequestToSend(false);
//	return true;

//	return write(fd,data.constData(),data.size())==data.size();
	//qDebug()<<"RAW_WRITE:"<<data;
	ttyPort->write(data);
	return true;
}

QString SerialDevice::portName()const
{
	return ttyPort->portName();
}

void SerialDevice::onNewData(const QByteArray &data)
{
	if(!data.isEmpty())
		streamParser.pushData(data);
}

void SerialDevice::onDevDirChanged()
{
	if(isConnected()&&!QFile::exists("/dev/"+ttyPort->portName()))
		closeTty();
}

void SerialDevice::onPortError()
{
	if(ttyPort->errorCode()!=SerialDriver::NoError&&ttyPort->errorCode()!=SerialDriver::UnknownError)
	{
		qDebug()<<"Closing tty port";
		closeTty();
	}
}

void SerialDevice::tryOpen()
{
	if(isConnected())
		return;
	if(!ttyPort->open())
		return;
	ttyPort->startReader();
	onConnected();
	identify();
}

void SerialDevice::syncFailed()
{
	closeTty();
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

void SerialDevice::closeTty()
{
	if(ttyPort->isOpened()||isConnected())
	{
		ttyPort->close();
		onDisconnected();
//		file->close();
//		close(fd);
//		fd=-1;
	}
}

void SerialDevice::setBaudRate(quint32 r)
{
	ttyPort->setBaudRate(r);
}

//void ARpcTtyDevice::setupSerialPort()
//{
//	//терминальная магия
//	termios t;
//	usleep(100*1000);
//	if(tcgetattr(fd,&t))return;//ниасилил терминальную магию
//	usleep(100*1000);
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
