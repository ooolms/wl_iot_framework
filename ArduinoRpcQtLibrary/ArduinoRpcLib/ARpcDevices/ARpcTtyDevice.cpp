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
	:ARpcDevice(parent)
{
//	watcher.addPath(info.absolutePath());
	connectedFlag=false;
//	file=0;
//	notif=0;
	QSerialPortInfo pInfo(portName);
	ttyPort=new QSerialPort(pInfo,this);
	reconnectTimer.setInterval(1000);
	reconnectTimer.setSingleShot(false);

//	connect(&watcher,SIGNAL(fileChanged(QString)),this,SLOT(onWatcherFileChanged(QString)));
//	connect(&watcher,SIGNAL(directoryChanged(QString)),this,SLOT(onWatcherDirChanged(QString)));
	connect(ttyPort,&QSerialPort::readyRead,this,&ARpcTtyDevice::onReadyRead);
	connect(ttyPort,static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
		this,&ARpcTtyDevice::onPortError);
	connect(&reconnectTimer,&QTimer::timeout,this,&ARpcTtyDevice::tryOpen);

	tryOpen();
	if(!connectedFlag)reconnectTimer.start();
}

ARpcTtyDevice::~ARpcTtyDevice()
{
	if(connectedFlag)closeTty();
}

bool ARpcTtyDevice::writeMsg(const ARpcMessage &m)
{
	if(!connectedFlag)return false;
	QByteArray data=(msgParser.dump(m)+ARpcConfig::msgDelim).toUtf8();
	return ttyPort->write(data)==data.size();
}

bool ARpcTtyDevice::isConnected()
{
	return connectedFlag;
}

QString ARpcTtyDevice::portName()const
{
	return ttyPort->portName();
}

void ARpcTtyDevice::onReadyRead()
{
	QByteArray data=ttyPort->readAll();
	if(!data.isEmpty())streamParser.pushData(QString::fromUtf8(data));
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
	if(ttyPort->isOpen())return;
	reconnectTimer.stop();
	if(!ttyPort->open(QIODevice::ReadWrite))
	{
		reconnectTimer.start();
		return;
	}
	QThread::msleep(1000);
	setupSerialPort();
	streamParser.reset();

	connectedFlag=true;
	emit connected();
	QByteArray data=ttyPort->readAll();
	if(!data.isEmpty())streamParser.pushData(QString::fromUtf8(data));
}

void ARpcTtyDevice::closeTty()
{
    if(ttyPort->isOpen())ttyPort->close();
	connectedFlag=false;
	emit disconnected();
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
}
