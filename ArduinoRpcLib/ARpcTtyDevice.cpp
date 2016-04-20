#include "ARpcTtyDevice.h"
#include <QFileInfo>
#include <fcntl.h>
#include <unistd.h>
#include <QDebug>
#include <ttyent.h>
#include <sys/ioctl.h>
#include <termios.h>

ARpcTtyDevice::ARpcTtyDevice(const QString &path,const ARpcConfig &cfg,QObject *parent)
	:ARpcDevice(cfg,parent)
	,msgParser(cfg)
	,streamParser(cfg,&msgParser)
{
	QFileInfo info(path);
	ttyPath=info.absoluteFilePath();
	watcher.addPath(path);
	watcher.addPath(info.absolutePath());
	connected=false;
	file=0;
	notif=0;

	connect(&watcher,SIGNAL(fileChanged(QString)),this,SLOT(onWatcherFileChanged(QString)));
	connect(&watcher,SIGNAL(directoryChanged(QString)),this,SLOT(onWatcherDirChanged(QString)));
	connect(&streamParser,SIGNAL(processMessage(ARpcMessage)),this,SLOT(processMessage(ARpcMessage)));

	tryOpen();
}

ARpcTtyDevice::~ARpcTtyDevice()
{
	if(connected)closeTty();
}

bool ARpcTtyDevice::writeMsg(const ARpcMessage &m)
{
	if(!connected)return false;
	QByteArray data=(msgParser.dump(m)+config.msgDelim).toUtf8();
	return file->write(data)==data.size();
}

bool ARpcTtyDevice::writeMsg(const QString &msg)
{
	if(!connected)return false;
	QByteArray data=(msg+config.msgDelim).toUtf8();
	return file->write(data)==data.size();
}

bool ARpcTtyDevice::writeMsg(const QString &msg,const QStringList &args)
{
	if(!connected)return false;
	QByteArray data=(msgParser.dump(ARpcMessage(msg,args))+config.msgDelim).toUtf8();
	return file->write(data)==data.size();
}

bool ARpcTtyDevice::isConnected()
{
	return connected;
}

void ARpcTtyDevice::onWatcherFileChanged(const QString &filePath)
{
	if(filePath==ttyPath)
	{
		QFileInfo info(ttyPath);
		if(connected&&!info.exists())
		{
			closeTty();
		}
	}
}

void ARpcTtyDevice::onWatcherDirChanged(const QString &dirPath)
{
	Q_UNUSED(dirPath)//only one dir watched
	QFileInfo info(ttyPath);
	if(!connected&&info.exists())
	{
		tryOpen();
	}
	else if(connected&&!info.exists())
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
	setupSerialPort();
	streamParser.reset();

//	fcntl(fd,F_SETFL,O_NONBLOCK);
	file=new QFile(this);
	file->open(fd,QIODevice::ReadWrite|QIODevice::Unbuffered);
	notif=new QSocketNotifier(fd,QSocketNotifier::Read,this);
	connect(notif,SIGNAL(activated(int)),this,SLOT(onReadyRead()));
	connected=true;
	emit ttyConnected();
	QByteArray data=file->readAll();
	if(!data.isEmpty())streamParser.pushData(QString::fromUtf8(data));
}

void ARpcTtyDevice::closeTty()
{
	file->close();
	delete file;
	delete notif;
	close(fd);
	connected=false;
	emit ttyDisconnected();
}

void ARpcTtyDevice::setupSerialPort()
{
	//терминальная магия
	termios t;
	if(tcgetattr(fd,&t))return;//ниасилил терминальную магию
	cfsetspeed(&t,B9600);
	//делаем как после ide
	t.c_iflag=0;
	t.c_oflag=0x4;
	t.c_cflag=0xcbd;
	t.c_lflag=0;
	t.c_line=0;
	tcsetattr(fd,TCSANOW,&t);
}
