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

#include "ARpcSerialDevice.h"
#include "ARpcSerialNotificator.h"
#include <QFileInfo>
#include <QDebug>
#include <QSerialPortInfo>
#include <QThread>

ARpcSerialDevice::ARpcSerialDevice(const QString &portName,QObject *parent)
	:ARpcRealDevice(parent)
{
	reconnectTimer.setInterval(1000);
	reconnectTimer.setSingleShot(false);

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
	ttyPort=new ARpcSerialDriver(portName,this);

	connect(&reconnectTimer,&QTimer::timeout,this,&ARpcSerialDevice::tryOpen);
	connect(&ARpcSerialNotificator::inst(),&ARpcSerialNotificator::checkSerialPorts,
		this,&ARpcSerialDevice::onDevDirChanged);
	connect(ttyPort,&ARpcSerialDriver::readyRead,this,&ARpcSerialDevice::onReadyRead);
	connect(ttyPort,&ARpcSerialDriver::error,this,&ARpcSerialDevice::onPortError);

//	tryOpen();
}

ARpcSerialDevice::~ARpcSerialDevice()
{
	closeTty();
}

bool ARpcSerialDevice::writeMsg(const ARpcMessage &m)
{
	if(!ttyPort->isOpened())
		return false;
//	struct stat s;
//	if(fd==-1||fstat(fd,&s)!=0)
//	{
//		return false;
//	}
	QByteArray data=ARpcStreamParser::dump(m);
//	ttyPort->setRequestToSend(true);
//	if(ttyPort->write(data)!=data.size())return false;
//	if(!ttyPort->flush())return false;
//	ttyPort->setRequestToSend(false);
//	return true;

//	return write(fd,data.constData(),data.size())==data.size();
	//qDebug()<<"RAW_WRITE:"<<data;
	return ttyPort->write(data)==data.size();
}

bool ARpcSerialDevice::isConnected()
{
	return ttyPort->isOpened();
//	struct stat s;
//	return fd!=-1&&fstat(fd,&s)==0;
}

QString ARpcSerialDevice::portName() const
{
	return ttyPort->portName();
}

void ARpcSerialDevice::onReadyRead()
{
//	QByteArray data=ttyPort->readAll();
	QByteArray data=ttyPort->readAll();
	//qDebug()<<"RAW_READ:"<<data;
	if(!data.isEmpty())
		streamParser.pushData(data);
}

void ARpcSerialDevice::onDevDirChanged()
{
	if(isConnected()&&!QFile::exists("/dev/"+ttyPort->portName()))
	{
		closeTty();
		reconnectTimer.start();
	}
}

void ARpcSerialDevice::onPortError()
{
//	qDebug()<<"Tty port error: "<<err<<":"<<ttyPort->errorString();
//	if(err==QSerialPort::DeviceNotFoundError/*||err==QSerialPort::PermissionError*/||err==QSerialPort::OpenError||
//		err==QSerialPort::NotOpenError||err==QSerialPort::ResourceError||err==QSerialPort::UnsupportedOperationError||
//		err==QSerialPort::TimeoutError)
//	{
//		qDebug()<<"Closing tty port";
//		closeTty();
//		reconnectTimer.start();
//	}
	//IMPL
	if(ttyPort->errorCode()!=ARpcSerialDriver::NoError&&ttyPort->errorCode()!=ARpcSerialDriver::UnknownError)
	{
		qDebug()<<"Closing tty port";
		closeTty();
		reconnectTimer.start();
	}
}

void ARpcSerialDevice::tryOpen()
{
	if(isConnected())
		return;
	reconnectTimer.stop();
	QThread::msleep(200);
	if(!ttyPort->open())
	{
		reconnectTimer.start();
		return;
	}

//	if(!ttyPort->open(QIODevice::ReadWrite))
//	{
//		qDebug()<<"Port open error: "<<ttyPort->errorString();
//		reconnectTimer.start();
//		return;
//	}
//	fd=open(("/dev/"+mPortName.toUtf8()).constData(),O_RDWR|O_NOCTTY|O_SYNC);
//	if(fd==-1)
//	{
//		reconnectTimer.start();
//		return;
//	}
//	if(notif)delete notif;
//	notif=new QSocketNotifier(fd,QSocketNotifier::Read,this);
//	connect(notif,&QSocketNotifier::activated,this,&ARpcTtyDevice::onReadyRead);
//	file->open(fd,QIODevice::ReadWrite|QIODevice::Unbuffered);
//	QThread::msleep(300);
//	setupSerialPort();
	emit connected();

//	ttyPort->setDataTerminalReady(true);
//	QByteArray data=ttyPort->readAll();
//	QByteArray data=file->readAll();
	QByteArray data=ttyPort->readAll();
//	qDebug()<<"RAW:"<<data;

	if(!data.isEmpty())
	{
		//qDebug()<<"RAW_READ:"<<data;
		streamParser.pushData(data);
	}
}

void ARpcSerialDevice::syncFailed()
{
	closeTty();
	reconnectTimer.start();
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

void ARpcSerialDevice::closeTty()
{
	if(isConnected())
	{
		ttyPort->close();
//		file->close();
//		close(fd);
//		fd=-1;
	}
	emit disconnected();
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
