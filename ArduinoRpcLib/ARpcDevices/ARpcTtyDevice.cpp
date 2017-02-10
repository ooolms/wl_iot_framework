#include "ARpcTtyDevice.h"
#include <QFileInfo>
#include <fcntl.h>
#include <unistd.h>
#include <QDebug>
#include <ttyent.h>
#include <sys/ioctl.h>
#include <termios.h>

ARpcTtyDevice::ARpcTtyDevice(const QString &path,QObject *parent)
	:ARpcDevice(parent)
{
	QFileInfo info(path);
	ttyPath=info.absoluteFilePath();
	watcher.addPath(info.absolutePath());
	connectedFlag=false;
	file=0;
	notif=0;

	connect(&watcher,SIGNAL(fileChanged(QString)),this,SLOT(onWatcherFileChanged(QString)));
	connect(&watcher,SIGNAL(directoryChanged(QString)),this,SLOT(onWatcherDirChanged(QString)));

	tryOpen();
}

ARpcTtyDevice::~ARpcTtyDevice()
{
	if(connectedFlag)closeTty();
}

bool ARpcTtyDevice::writeMsg(const ARpcMessage &m)
{
	if(!connectedFlag)return false;
	QByteArray data=(msgParser.dump(m)+ARpcConfig::msgDelim).toUtf8();
	return file->write(data)==data.size();
}

bool ARpcTtyDevice::isConnected()
{
	return connectedFlag;
}

void ARpcTtyDevice::onWatcherFileChanged(const QString &filePath)
{
	if(filePath==ttyPath)
	{
		QFileInfo info(ttyPath);
		if(connectedFlag&&!info.exists())
		{
			closeTty();
		}
	}
}

void ARpcTtyDevice::onWatcherDirChanged(const QString &dirPath)
{
	Q_UNUSED(dirPath)//only one dir watched
	QFileInfo info(ttyPath);
	if(!connectedFlag&&info.exists())
	{
		tryOpen();
	}
	else if(connectedFlag&&!info.exists())
	{
		closeTty();
	}
}

void ARpcTtyDevice::onReadyRead()
{
	QByteArray data=file->readAll();
	if(!data.isEmpty())streamParser.pushData(QString::fromUtf8(data));
}

void ARpcTtyDevice::tryOpen()
{
	fd=open(ttyPath.toUtf8().constData(),O_RDWR|O_NOCTTY|O_NONBLOCK);
	if(fd==-1)return;
	usleep(1000*1000);
	setupSerialPort();
	streamParser.reset();

//	fcntl(fd,F_SETFL,O_NONBLOCK);
	file=new QFile(this);
	file->open(fd,QIODevice::ReadWrite|QIODevice::Unbuffered);
	notif=new QSocketNotifier(fd,QSocketNotifier::Read,this);
	connect(notif,SIGNAL(activated(int)),this,SLOT(onReadyRead()));
	connectedFlag=true;
	emit connected();
	QByteArray data=file->readAll();
	if(!data.isEmpty())streamParser.pushData(QString::fromUtf8(data));
	usleep(1000*1000);
}

void ARpcTtyDevice::closeTty()
{
	file->close();
	delete file;
	delete notif;
	close(fd);
	connectedFlag=false;
	emit disconnected();
}

void ARpcTtyDevice::setupSerialPort()
{
	//терминальная магия
	termios t;
	usleep(100*1000);
	if(tcgetattr(fd,&t))return;//ниасилил терминальную магию
	usleep(100*1000);
	cfsetspeed(&t,B9600);
	//делаем как после ide
	t.c_iflag=0;
	t.c_oflag=0;
	t.c_cflag=CS8|B9600|CREAD|CLOCAL|HUPCL;
	t.c_lflag=0;
	t.c_line=0;
	tcsetattr(fd,TCSANOW,&t);
}
