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

#include "SerialDriver.h"
#include <QThread>
#include <QDebug>
#include <QMutexLocker>
#include <QEventLoop>

#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#include <ttyent.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <termios.h>
#include <time.h>
#endif

#ifdef Q_OS_WIN
#else
#endif

/* linux example
int RTS_flag;
RTS_flag = TIOCM_RTS;
ioctl(fd,TIOCMBIS,&RTS_flag);//Set RTS pin
getchar();
ioctl(fd,TIOCMBIC,&RTS_flag);//Clear RTS pin
*/

using namespace WLIOT;

CommReader::CommReader(FileDescrType f,QObject *parent)
	:QThread(parent)
{
	fd=f;
	t=0;
#ifdef Q_OS_WIN
	rs.hEvent=CreateEventA(0,FALSE,FALSE,0);
	ws.hEvent=CreateEventA(0,FALSE,FALSE,0);
	rs.Internal=rs.InternalHigh=0;
	ws.Internal=ws.InternalHigh=0;
#endif
}

CommReader::~CommReader()
{
	terminate();
	wait();
#ifdef Q_OS_WIN
	CloseHandle(rs.hEvent);
	CloseHandle(ws.hEvent);
#endif
}

void CommReader::writeData(const QByteArray &data)
{
	QMutexLocker l(&m);
	wData.append(data);
}

void CommReader::run()
{
	if(t)delete t;
	t=new QTimer;
	t->setInterval(100);
	t->setSingleShot(true);
	connect(t,&QTimer::timeout,this,&CommReader::onTimer);

	char c;
	QByteArray dd;
	QEventLoop loop;
#ifdef Q_OS_WIN
	rs.Offset=rs.OffsetHigh=0;
	DWORD br;
	bool waitForEvent=false;
#else
	fd_set s;
	timeval ts;
	ts.tv_sec=0;
	ts.tv_usec=100000;
	int selRVal;
#endif

	while(!isInterruptionRequested())
	{
		dd.clear();
#ifdef Q_OS_WIN
		if(!waitForEvent)
		{
			for(int i=0;i<4096;++i)
			{
				rs.Offset=rs.OffsetHigh=0;
				if(ReadFile(fd,&c,1,&br,&rs))
				{
					if(br>0)
						dd.append(c);
					else break;
				}
				else
				{
					if(GetLastError()==ERROR_IO_PENDING)
					{
						waitForEvent=true;
						break;
					}
					else if(!isInterruptionRequested()) emit readError();
				}
				break;
			}
		}
		else
		{
			if(WaitForSingleObject(rs.hEvent,100))
				waitForEvent=false;
		}
#else
		FD_ZERO(&s);
		FD_SET(fd,&s);
		selRVal=select(fd+1,&s,0,0,&ts);
		if(selRVal==-1)
			emit readError();
		else if(selRVal)
		{
			ssize_t sz=0;
			for(int i=0;i<4096;++i)
			{
				sz=::read(fd,&c,1);
				if(sz<0)
				{
					if(!isInterruptionRequested())
						emit readError();
					break;
				}
				else if(sz>0)
					dd.append(c);
				else break;
			}
		}
#endif
		if(!dd.isEmpty())
		{
			QMutexLocker l(&m);
			rData.append(dd);
			if(!t->isActive())
				t->start();
		}
		writePendingData();
		loop.processEvents();
	}
	delete t;
	t=0;
}

void CommReader::writePendingData()
{
	QMutexLocker l(&m);
	if(wData.isEmpty())return;
//	qDebug()<<"RAW SERIAL WRITE: "<<wData;
#ifdef Q_OS_WIN
	DWORD sz=0;
	ws.Offset=ws.OffsetHigh=0;
	if(!WriteFile(fd,wData.constData(),wData.size(),&sz,&ws))
	{
		DWORD e=GetLastError();
		if(e==ERROR_IO_PENDING)
		{
			if(!WaitForSingleObject(ws.hEvent,INFINITE))
				emit writeError();
		}
		else
		{
			qDebug()<<"WIN ERR: "<<QByteArray::number((uint)e)<<" (0x"<<QString::number((uint)e,16)<<")";
			emit writeError();
		}
	}

#else
	int arg=TIOCM_RTS;
	ioctl(fd,TIOCMBIS,&arg);
	qint64 sz=::write(fd,wData.constData(),wData.size());
	tcdrain(fd);
	arg=TIOCM_RTS;
	ioctl(fd,TIOCMBIC,&arg);
	if(sz!=wData.size())
		emit writeError();
#endif
	wData.clear();
}

void CommReader::onTimer()
{
	QMutexLocker l(&m);
//	qDebug()<<"RAW SERIAL READ: "<<rData;
	if(!rData.isEmpty())
	{
		QByteArray d;
		std::swap(d,rData);
		emit newData(std::move(d));
	}
}

SerialDriver::SerialDriver(const QString &portName,const QString &connOptions,QObject *parent)
	:QObject(parent)
{
	mPortName=portName;
	bRate=BRate9600;
	flowControl=NoFlowControl;
#ifdef Q_OS_WIN
	fd=INVALID_HANDLE_VALUE;
	dataBits=8;
	stopBits=ONESTOPBIT;
	parity=NOPARITY;
#else
	fd=-1;
	dataBitsFlag=CS8;
	stopBitsFlag=0;
	parityFlag=0;
#endif
	lastError=NoError;
	QStringList optList=connOptions.split(':');
	bool ok=false;
	if(optList.count()>=1)
	{
		quint32 v=optList[0].toUInt(&ok);
		if(ok)setBRate(v);
	}
	if(optList.count()>=2)
	{
		quint8 v=(quint8)optList[1].toUShort(&ok);
		if(ok)setDataBits(v);
	}
	if(optList.count()>=3)
	{
		quint8 v=(quint8)optList[2].toUShort(&ok);
		if(ok)setStopBits(v);
	}
	if(optList.count()>=4)
		setParity(optList[3]);
	if(optList.count()>=5)
		setFlowControl(optList[4]);
}

SerialDriver::~SerialDriver()
{
	close();
}

bool SerialDriver::open()
{
	if(isOpened())return false;
#ifdef Q_OS_WIN
	fd=CreateFileA(("\\\\.\\"+mPortName.toUtf8()).constData(),
		GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
	if(fd==INVALID_HANDLE_VALUE)
	{
		lastError=AccessError;
		emit error();
		return false;
	}
//	QThread::msleep(500);
#else
	fd=::open(("/dev/"+mPortName.toUtf8()).constData(),O_RDWR|O_NOCTTY|O_NDELAY|O_SYNC);
	if(fd==-1)
	{
		lastError=AccessError;
		emit error();
		return false;
	}
	struct stat s;
	while(fstat(fd,&s)==-1)
		qDebug()<<".";
//	QThread::msleep(100);
#endif
	setupSerialPort();
	reader=new CommReader(fd,this);
	connect(reader,SIGNAL(newData(QByteArray)),this,SIGNAL(newData(QByteArray)),Qt::QueuedConnection);
	return true;
}

bool SerialDriver::isOpened()
{
#ifdef Q_OS_WIN
	return fd!=INVALID_HANDLE_VALUE;
#else
	struct stat s;
	return fd!=-1&&fstat(fd,&s)==0;
#endif
}

void SerialDriver::close()
{
	if(!isOpened())return;
//	mFile.close();
	reader->requestInterruption();
#ifdef Q_OS_WIN
	CloseHandle(fd);
	fd=INVALID_HANDLE_VALUE;
#else
	::close(fd);
	fd=-1;
#endif
	reader->wait(1000);
	delete reader;
}

SerialDriver::Error SerialDriver::errorCode()
{
	return lastError;
}

QString SerialDriver::errorString()
{
	if(lastError==AccessError)
		return "Access error";
	else if(lastError==ReadError)
		return "Read error";
	else if(lastError==WriteError)
		return "Write error";
	else if(lastError==UnknownError)
		return "Unknown error";
	return QString();
}

void SerialDriver::write(const QByteArray &data)
{
	reader->writeData(data);
}

QString SerialDriver::portName()
{
	return mPortName;
}

void SerialDriver::startReader()
{
	reader->start();
}

void SerialDriver::setupSerialPort()
{
#ifdef Q_OS_WIN
	DCB dcb;
	GetCommState(fd,&dcb);
	dcb.ByteSize=dataBits;
	dcb.Parity=parity;
	dcb.StopBits=stopBits;
	dcb.BaudRate=bRate;
	if(flowControl==HwFlowControl)
	{
		dcb.fDtrControl=DTR_CONTROL_ENABLE;
		dcb.fRtsControl=RTS_CONTROL_ENABLE;
	}
	else if(flowControl==SwFlowControl)
	{
		dcb.fInX=TRUE;
		dcb.fOutX=TRUE;
	}
	SetCommState(fd,&dcb);
	COMMTIMEOUTS tms;
	GetCommTimeouts(fd,&tms);
	tms.ReadIntervalTimeout=MAXDWORD;
//	tms.ReadIntervalTimeout=10;
	tms.ReadTotalTimeoutConstant=0;
	tms.ReadTotalTimeoutMultiplier=0;
	SetCommTimeouts(fd,&tms);
	EscapeCommFunction(fd,SETDTR);
#else
	termios t;
	speed_t spd=bRate;
	if(tcgetattr(fd,&t))return;//ниасилил терминальную магию
	t.c_iflag=0;
	t.c_oflag=0;
	t.c_cflag=CREAD|CLOCAL|HUPCL|dataBitsFlag|stopBitsFlag|parityFlag;
	t.c_lflag=0;
	t.c_line=0;
	if(flowControl==HwFlowControl)
		t.c_cflag|=CRTSCTS;
	else if(flowControl==SwFlowControl)
	{
		t.c_iflag|=IXON;
		t.c_iflag|=IXOFF;
	}
	cfmakeraw(&t);
	cfsetspeed(&t,spd);
	tcsetattr(fd,TCSANOW,&t);
	int arg=TIOCM_DTR;
	ioctl(fd,TIOCMBIS,&arg);
#endif
}

void SerialDriver::setBRate(quint32 valueFromStr)
{
	if(valueFromStr==300)
		bRate=BRate300;
	else if(valueFromStr==600)
		bRate=BRate600;
	else if(valueFromStr==1200)
		bRate=BRate1200;
	else if(valueFromStr==2400)
		bRate=BRate2400;
	else if(valueFromStr==4800)
		bRate=BRate4800;
	else if(valueFromStr==19200)
		bRate=BRate19200;
	else if(valueFromStr==38400)
		bRate=BRate38400;
	else if(valueFromStr==57600)
		bRate=BRate57600;
	else if(valueFromStr==115200)
		bRate=BRate115200;
	else if(valueFromStr==230400)
		bRate=BRate230400;
	else if(valueFromStr==460800)
		bRate=BRate460800;
	else if(valueFromStr==921600)
		bRate=BRate921600;
	else if(valueFromStr==1500000)
		bRate=BRate1500000;
	else if(valueFromStr==2000000)
		bRate=BRate2000000;
	else if(valueFromStr==2500000)
		bRate=BRate2500000;
	else if(valueFromStr==3000000)
		bRate=BRate3000000;
	else if(valueFromStr==3500000)
		bRate=BRate3500000;
	else if(valueFromStr==4000000)
		bRate=BRate4000000;
	else bRate=BRate9600;
}

void SerialDriver::setDataBits(quint8 valueFromStr)
{
#ifdef Q_OS_WIN
	if(valueFromStr==5)
		dataBits=5;
	else if(valueFromStr==6)
		dataBits=6;
	else if(valueFromStr==7)
		dataBits=7;
	else dataBits=8;
#else
	if(valueFromStr==5)
		dataBitsFlag=CS5;
	else if(valueFromStr==6)
		dataBitsFlag=CS6;
	else if(valueFromStr==7)
		dataBitsFlag=CS7;
	else dataBitsFlag=CS8;
#endif
}

void SerialDriver::setStopBits(quint8 valueFromStr)
{
#ifdef Q_OS_WIN
	if(valueFromStr==2)
		stopBits=2;
	else stopBits=1;
#else
	if(valueFromStr==2)
		stopBitsFlag=CSTOPB;
	else stopBitsFlag=0;
#endif
}

void SerialDriver::setParity(const QString &str)
{
#ifdef Q_OS_WIN
	if(str=="odd")
		parity=ODDPARITY;
	else if(str=="even")
		parity=EVENPARITY;
	else if(str=="mark")
		parity=MARKPARITY;
	else if(str=="space")
		parity=SPACEPARITY;
	else parity=NOPARITY;
#else
	if(str=="odd")
		parityFlag=PARENB|PARODD;
	else if(str=="even")
		parityFlag=PARENB;
	else if(str=="mark")
		parityFlag=PARENB|PARODD|CMSPAR;
	else if(str=="space")
		parityFlag=PARENB|CMSPAR;
	else parityFlag=0;
#endif
}

void SerialDriver::setFlowControl(const QString &str)
{
	if(str=="hw")
		flowControl=HwFlowControl;
	else if(str=="sw")
		flowControl=SwFlowControl;
	else flowControl=NoFlowControl;
}
