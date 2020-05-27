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

SerialDriver::SerialDriver(const QString &portName,QObject *parent)
	:QObject(parent)
{
	mPortName=portName;
	bRate=9600;
#ifdef Q_OS_WIN
	fd=INVALID_HANDLE_VALUE;
#else
	fd=-1;
#endif
	lastError=NoError;
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

void SerialDriver::setBaudRate(quint32 r)
{
	bRate=r;
}

void SerialDriver::setupSerialPort()
{
#ifdef Q_OS_WIN
	DCB dcb;
	GetCommState(fd,&dcb);
	dcb.BaudRate=CBR_9600;
	dcb.ByteSize=8;
	dcb.Parity=NOPARITY;
	dcb.StopBits=ONESTOPBIT;
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
	speed_t spd=B9600;
	if(bRate==19200)
		spd=B19200;
	else if(bRate==38400)
		spd=B38400;
	else if(bRate==115200)
		spd=B115200;
	else if(bRate==460800)
		spd=B460800;
	if(tcgetattr(fd,&t))return;//ниасилил терминальную магию
	t.c_iflag=0;
	t.c_oflag=0;
	t.c_cflag=CS8|CREAD|CLOCAL|HUPCL;
	t.c_lflag=0;
	t.c_line=0;
	cfmakeraw(&t);
	cfsetspeed(&t,spd);
	tcsetattr(fd,TCSANOW,&t);
	int arg=TIOCM_DTR;
	ioctl(fd,TIOCMBIS,&arg);
#endif
}
