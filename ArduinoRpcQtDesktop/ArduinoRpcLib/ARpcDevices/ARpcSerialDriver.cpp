#include "ARpcSerialDriver.h"
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

//CRIT
//set com port DTR,RTS: SetCommMask on win, ioctl on linux

/* linux example
int RTS_flag;
RTS_flag = TIOCM_RTS;
ioctl(fd,TIOCMBIS,&RTS_flag);//Set RTS pin
getchar();
ioctl(fd,TIOCMBIC,&RTS_flag);//Clear RTS pin
*/

ARpcCommReader::ARpcCommReader(FileDescrType f,QObject *parent)
	:QThread(parent)
{
	fd=f;
	t=0;
}

ARpcCommReader::~ARpcCommReader()
{
	terminate();
	wait();
}

void ARpcCommReader::run()
{
	if(t)delete t;
	t=new QTimer;
	t->setInterval(100);
	t->setSingleShot(true);
	connect(t,&QTimer::timeout,this,&ARpcCommReader::onTimer);

	char c;
	QByteArray dd;
	QEventLoop loop;
#ifdef Q_OS_WIN
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
		//IMPL
#else
		FD_ZERO(&s);
		FD_SET(fd,&s);
		selRVal=select(fd+1,&s,0,0,&ts);
		if(selRVal==-1)
			emit readError();
		else if(selRVal)
		{
			ssize_t sz=0;
			while(true)
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
			data.append(dd);
			if(!t->isActive())
				t->start();
		}
		loop.processEvents();
	}
	delete t;
	t=0;
}

void ARpcCommReader::onTimer()
{
	QMutexLocker l(&m);
	emit newData(data);
	data.clear();
}

ARpcSerialDriver::ARpcSerialDriver(const QString &portName,QObject *parent)
	:QObject(parent)
{
	mPortName=portName;
#ifdef Q_OS_WIN
	fd=INVALID_HANDLE_VALUE;
#else
	fd=-1;
#endif
	lastError=NoError;
}

ARpcSerialDriver::~ARpcSerialDriver()
{
	close();
}

bool ARpcSerialDriver::open()
{
	if(isOpened())return false;
#ifdef Q_OS_WIN
	fd=CreateFileA(("\\\\.\\"+mPortName.toUtf8()).constData(),
		GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
	if(fd==INVALID_HANDLE_VALUE)
	{
		lastError=AccessError;
		emit error();
		return false;
	}
	QThread::msleep(500);
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
	QThread::msleep(500);
#endif
	setupSerialPort();
	reader=new ARpcCommReader(fd,this);
	connect(reader,&ARpcCommReader::newData,this,&ARpcSerialDriver::newData,Qt::QueuedConnection);
	return true;
}

bool ARpcSerialDriver::isOpened()
{
#ifdef Q_OS_WIN
	return fd!=INVALID_HANDLE_VALUE;
#else
	struct stat s;
	return fd!=-1&&fstat(fd,&s)==0;
#endif
}

void ARpcSerialDriver::close()
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

ARpcSerialDriver::Error ARpcSerialDriver::errorCode()
{
	return lastError;
}

QString ARpcSerialDriver::errorString()
{
	if(lastError==AccessError)
		return "Access error";
	else if(lastError==ReadError)
		return "Read error";
	else if(lastError==WriteError)
		return "Write error";
	return "Unknown error";
}

qint64 ARpcSerialDriver::write(const QByteArray &data)
{
#ifdef Q_OS_WIN
#else
	int arg=TIOCM_RTS;
	ioctl(fd,TIOCMBIS,&arg);
#endif
#ifdef Q_OS_WIN
	unsigned long sz=0;
	//EscapeCommFunction(fd,SETRTS);
	WriteFile(fd,data.constData(),data.size(),&sz,0);
	/*DWORD eMask,ev;
	GetCommMask(fd,&eMask);
	SetCommMask(fd,EV_TXEMPTY);
	WaitCommEvent(fd,&ev,0);
	SetCommMask(fd,eMask);
	EscapeCommFunction(fd,CLRRTS);*/
#else
	qint64 sz=::write(fd,data.constData(),data.size());
	tcdrain(fd);
	arg=TIOCM_RTS;
	ioctl(fd,TIOCMBIC,&arg);
#endif

	if(sz!=data.size())
	{
		lastError=WriteError;
		emit error();
	}
	return sz;
}

QString ARpcSerialDriver::portName()
{
	return mPortName;
}

void ARpcSerialDriver::startReader()
{
	reader->start();
}

void ARpcSerialDriver::setupSerialPort()
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
	if(tcgetattr(fd,&t))return;//ниасилил терминальную магию
//	t.c_iflag=0;
//	t.c_oflag=0;
//	t.c_cflag=CS8|B9600|CREAD|CLOCAL|HUPCL;
//	t.c_lflag=0;
//	t.c_line=0;
	cfmakeraw(&t);
	cfsetspeed(&t,B9600);
	tcsetattr(fd,TCSANOW,&t);
	int arg=TIOCM_DTR;
	ioctl(fd,TIOCMBIS,&arg);
#endif
}
