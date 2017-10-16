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

#include "ARpcTtyDevice.h"
#include <QFileInfo>
#include <QDebug>
//#include <fcntl.h>
//#include <unistd.h>
//#include <ttyent.h>
//#include <sys/ioctl.h>
//#include <termios.h>
#include <QSerialPortInfo>
#include <QThread>

ARpcTtyDevice::ARpcTtyDevice(const QString &portName,QObject *parent)
	:ARpcRealDevice(parent)
{
	connectedFlag=false;
	QSerialPortInfo pInfo(portName);
	ttyPort=new QSerialPort(pInfo,this);
	reconnectTimer.setInterval(1000);
	reconnectTimer.setSingleShot(false);

	connect(ttyPort,&QSerialPort::readyRead,this,&ARpcTtyDevice::onReadyRead);
	connect(ttyPort,static_cast<void (QSerialPort::*)(
		QSerialPort::SerialPortError)>(&QSerialPort::error),this,
		&ARpcTtyDevice::onPortError);
	connect(&reconnectTimer,&QTimer::timeout,this,&ARpcTtyDevice::tryOpen);

	tryOpen();
	if(!connectedFlag)
		reconnectTimer.start();
}

ARpcTtyDevice::~ARpcTtyDevice()
{
	if(connectedFlag)
		closeTty();
}

bool ARpcTtyDevice::writeMsg(const ARpcMessage &m)
{
	if(!connectedFlag)
		return false;
	QByteArray data=(msgParser.dump(m)+ARpcConfig::msgDelim).toUtf8();
	if(ttyPort->write(data)!=data.size())return false;
	return ttyPort->flush();
}

bool ARpcTtyDevice::isConnected()
{
	return connectedFlag;
}

QString ARpcTtyDevice::portName() const
{
	return ttyPort->portName();
}

void ARpcTtyDevice::closePort()
{
	if(!connectedFlag)
		return;
	resetIdentification();
	connectedFlag=false;
	closeTty();
	emit disconnected();
}

void ARpcTtyDevice::onReadyRead()
{
	QByteArray data=ttyPort->readAll();
	if(!data.isEmpty())
		streamParser.pushData(QString::fromUtf8(data));
}

void ARpcTtyDevice::onPortError(QSerialPort::SerialPortError err)
{
	if(connectedFlag&&err!=QSerialPort::NoError)
	{
		closeTty();
		reconnectTimer.start();
		qDebug()<<"Tty port error: "<<ttyPort->errorString();
	}
}

void ARpcTtyDevice::tryOpen()
{
	if(ttyPort->isOpen())
		return;
	reconnectTimer.stop();
	if(!ttyPort->open(QIODevice::ReadWrite))
	{
		qDebug()<<"Port open error: "<<ttyPort->errorString();
		reconnectTimer.start();
		return;
	}
	QThread::msleep(1000);
	setupSerialPort();
	streamParser.reset();
	connectedFlag=true;
	emit connected();
	ttyPort->setDataTerminalReady(true);
	QByteArray data=ttyPort->readAll();
	if(!data.isEmpty())
		streamParser.pushData(QString::fromUtf8(data));
}

void ARpcTtyDevice::setBaudRate(qint32 rate,QSerialPort::Directions directions)
{
	ttyPort->setBaudRate(rate,directions);
}

void ARpcTtyDevice::setDataBits(QSerialPort::DataBits bits)
{
	ttyPort->setDataBits(bits);
}

void ARpcTtyDevice::setFlowControl(QSerialPort::FlowControl ctl)
{
	ttyPort->setFlowControl(ctl);
}

void ARpcTtyDevice::setParity(QSerialPort::Parity parity)
{
	ttyPort->setParity(parity);
}

void ARpcTtyDevice::setStopBits(QSerialPort::StopBits bits)
{
	ttyPort->setStopBits(bits);
}

qint32 ARpcTtyDevice::baudRate()
{
	return ttyPort->baudRate();
}

QSerialPort::DataBits ARpcTtyDevice::dataBits()
{
	return ttyPort->dataBits();
}

QSerialPort::FlowControl ARpcTtyDevice::flowControl()
{
	return ttyPort->flowControl();
}

QSerialPort::Parity ARpcTtyDevice::parity()
{
	return ttyPort->parity();
}

QSerialPort::StopBits ARpcTtyDevice::stopBits()
{
	return ttyPort->stopBits();
}

void ARpcTtyDevice::closeTty()
{
	connectedFlag=false;
	if(ttyPort->isOpen())
		ttyPort->close();
	emit disconnected();
	resetIdentification();
}

void ARpcTtyDevice::setupSerialPort()
{
	//терминальная магия
	//	termios t;
	//	usleep(100*1000);
	//	if(tcgetattr(fd,&t))return;//ниасилил терминальную магию
	//	usleep(100*1000);
	//	cfsetspeed(&t,B9600);
	//	//делаем как после ide
	//	t.c_iflag=0;
	//	t.c_oflag=0;
	//	t.c_cflag=CS8|B9600|CREAD|CLOCAL|HUPCL;
	//	t.c_lflag=0;
	//	t.c_line=0;
	//	tcsetattr(fd,TCSANOW,&t);
	//all are default values
	ttyPort->setBaudRate(QSerialPort::Baud9600);
	ttyPort->setDataBits(QSerialPort::Data8);
	ttyPort->setFlowControl(QSerialPort::NoFlowControl);
	ttyPort->setParity(QSerialPort::NoParity);
	ttyPort->setStopBits(QSerialPort::OneStop);
	//	ttyPort->setDataTerminalReady(true);
}
