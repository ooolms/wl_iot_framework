#include "ARpcSerialDriver.h"
#include <QThread>
#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#include <ttyent.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <termios.h>
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

//TODO read/write using linux/win api

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
	readNotif=new ARpcWinCommEventListener(fd,this);
	connect(readNotif,&ARpcWinCommEventListener::readyRead,this,&ARpcSerialDriver::readyRead,Qt::QueuedConnection);
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
	readNotif=new QSocketNotifier((qintptr)fd,QSocketNotifier::Read,this);
	connect(readNotif,&QSocketNotifier::activated,this,&ARpcSerialDriver::readyRead);
#endif
	//QThread::msleep(500);
	//exceptNotif=new QSocketNotifier((qintptr)fd,QSocketNotifier::Exception,this);
//	connect(exceptNotif,&QSocketNotifier::activated,this,&ARpcSerialDriver::error);
//	mFile.open(fd,QIODevice::ReadWrite);
	setupSerialPort();
	//QThread::msleep(500);
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
#ifdef Q_OS_WIN
	CloseHandle(fd);
	fd=INVALID_HANDLE_VALUE;
	delete readNotif;
#else
	::close(fd);
	fd=-1;
	delete readNotif;
#endif
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

QByteArray ARpcSerialDriver::readAll()
{
//	return mFile.readAll();
	QByteArray d,s;
	s.resize(4096);
	while(1)
	{
		s=read(4096);
		if(s.isEmpty())break;
		d+=s;
	}
	return d;
}

QByteArray ARpcSerialDriver::read(qint64 sz)
{
//	QByteArray d=mFile.read(sz);
//	if(d.size()!=sz)
//	{
//		lastError=ReadError;
//		emit error();
//	}
//	return d;
	QByteArray d;
	d.resize(sz);
	d.fill(0);
#ifdef Q_OS_WIN
	unsigned long sz2;
	if(!ReadFile(fd,d.data(),sz,&sz2,0))
	{
		lastError=ReadError;
		emit error();
	}
#else
	qint64 sz2=::read(fd,d.data(),sz);
	if(sz2==-1)
	{
		if(errno==EAGAIN||errno==EWOULDBLOCK)
			return QByteArray();
		lastError=ReadError;
		emit error();
	}
#endif
	d.resize(sz2);
	return d;
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

#ifdef Q_OS_WIN
ARpcWinCommEventListener::ARpcWinCommEventListener(HANDLE f,QObject *parent)
	:QThread(parent)
{
	fd=f;
	start();
}

ARpcWinCommEventListener::~ARpcWinCommEventListener()
{
	terminate();
	wait();
}

void ARpcWinCommEventListener::run()
{
	DWORD evt=0;
	unsigned long le=0;
	SetCommMask(fd,EV_RXCHAR);
	while(true)
	{
		if(WaitCommEvent(fd,&evt,0))
		{
			if(evt&EV_RXCHAR)
				emit readyRead();
		}
		else
		{
			le=GetLastError();
		}
		QThread::usleep(1);
	}
}
#endif
