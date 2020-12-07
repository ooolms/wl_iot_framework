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
#include <poll.h>
#include <sys/timerfd.h>
#include <sys/eventfd.h>
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
#ifdef Q_OS_WIN
	rs.hEvent=CreateEventA(0,FALSE,FALSE,0);
	ws.hEvent=CreateEventA(0,FALSE,FALSE,0);
	rs.Internal=rs.InternalHigh=0;
	ws.Internal=ws.InternalHigh=0;
#else
	writeEventFd=-1;
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
#ifdef Q_OS_WIN
#else
	if(writeEventFd!=-1)
		eventfd_write(writeEventFd,1);
#endif
}

void CommReader::run()
{
#ifdef Q_OS_WIN
	QTimer t;
	t.setInterval(100);
	t.setSingleShot(true);
	connect(&t,&QTimer::timeout,this,&CommReader::onTimer,Qt::QueuedConnection);

	QEventLoop loop;
	char c;
	QByteArray dd;
	rs.Offset=rs.OffsetHigh=0;
	DWORD br;
	bool waitForEvent=false;

	while(!isInterruptionRequested())
	{
		dd.clear();
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
		if(!dd.isEmpty())
		{
			QMutexLocker l(&m);
			rData.append(dd);
			if(!t.isActive())
				t.start();
		}
		writePendingData();
		loop.processEvents();
	}
#else
	char c[4096],readBuf[8];
	int timerFd=timerfd_create(CLOCK_REALTIME,0);
	writeEventFd=eventfd(0,0);
	pollfd fds[3];
	fds[0].fd=fd;
	fds[0].events=POLLIN;
	fds[1].fd=timerFd;
	fds[1].events=POLLIN;
	fds[2].fd=writeEventFd;
	fds[2].events=POLLIN;
	itimerspec ts;
	ts.it_value.tv_sec=0;
	ts.it_value.tv_nsec=200000000;//200 ms
	ts.it_interval.tv_sec=0;
	ts.it_interval.tv_nsec=0;
	int pollRVal;
	while(!isInterruptionRequested())
	{
		pollRVal=poll(fds,3,200);
		if(pollRVal==-1)
		{
			if(!isInterruptionRequested())
			{
				qDebug()<<"SERIAL POLL ERROR 1";
				emit readError();
			}
		}
		else if(pollRVal==0)
			continue;
		else if((fds[0].revents&POLLERR)||(fds[0].revents&POLLHUP)||(fds[0].revents&POLLNVAL))
		{
			if(!isInterruptionRequested())
			{
				qDebug()<<"SERIAL POLL ERROR 2";
				emit readError();
			}
		}
		else
		{
			if(fds[0].revents&POLLIN)
			{
				ssize_t sz=::read(fd,c,4096);
				if(sz<0&&errno==EAGAIN)
				{
					if(!isInterruptionRequested())
					{
						qDebug()<<"SERIAL RAW ERROR: "<<strerror(errno);
						emit readError();
					}
				}
				else if(sz>0)
				{
					QMutexLocker l(&m);
					rData.append(c,sz);
					timerfd_settime(timerFd,0,&ts,0);
				}
			}
			if(fds[2].revents&POLLIN)
			{
				::read(writeEventFd,readBuf,8);
				writePendingData();
			}
			if(fds[1].revents&POLLIN)
			{
				::read(timerFd,readBuf,8);
				QMetaObject::invokeMethod(this,"onTimer",Qt::QueuedConnection);
			}
		}
		fds[0].revents=0;
		fds[1].revents=0;
		fds[2].revents=0;
	}
	::close(timerFd);
	timerFd=-1;
	::close(writeEventFd);
	writeEventFd=-1;
#endif
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
	fd=::open(("/dev/"+mPortName.toUtf8()).constData(),O_RDWR|O_NOCTTY|O_NONBLOCK);
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
	while(!reader->isRunning())
		QThread::yieldCurrentThread();
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
