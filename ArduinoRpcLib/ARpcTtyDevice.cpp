#include "ARpcTtyDevice.h"
#include <QFileInfo>
#include <fcntl.h>
#include <unistd.h>
#include <QDebug>

ARpcTtyDevice::ARpcTtyDevice(const QString &path,const ARpcConfig &cfg,QObject *parent)
	:ARpcDevice(config,parent)
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
	connect(&parser,SIGNAL(processMessage(ARpcMessage)),this,SLOT(processMessage(ARpcMessage)));

	tryOpen();
}

ARpcTtyDevice::~ARpcTtyDevice()
{
	if(connected)closeTty();
}

bool ARpcTtyDevice::writeMsg(const ARpcMessage &m)
{
	if(!connected)return false;
	QByteArray data=m.dump().toUtf8();
	return file->write(data)==data.size();
}

bool ARpcTtyDevice::writeMsg(const QString &msg)
{
	if(!connected)return false;
	QByteArray data=msg.toUtf8();
	return file->write(data)==data.size();
}

bool ARpcTtyDevice::writeMsg(const QString &msg,const QStringList &args)
{
	if(!connected)return false;
	QByteArray data=ARpcMessage(msg,args).dump().toUtf8();
	return file->write(data)==data.size();
}

bool ARpcTtyDevice::isConnected()
{
	return connected;
}

void ARpcTtyDevice::onWatcherFileChanged(const QString &filePath)
{
//	qDebug()<<"onWatcherFileChanged:1 "<<filePath;
	if(filePath==ttyPath)
	{
		QFileInfo info(ttyPath);
		if(connected&&!info.exists())
		{
//			qDebug()<<"onWatcherFileChanged:2";
			closeTty();
		}
	}
}

void ARpcTtyDevice::onWatcherDirChanged(const QString &dirPath)
{
	Q_UNUSED(dirPath)//only one dir watched
	QFileInfo info(ttyPath);
//	qDebug()<<"onWatcherDirChanged:1 "<<connected;
	if(!connected&&info.exists())
	{
//		qDebug()<<"onWatcherDirChanged:2";
		tryOpen();
	}
	else if(connected&&!info.exists())
	{
//		qDebug()<<"onWatcherDirChanged:3";
		closeTty();
	}
}

void ARpcTtyDevice::onReadyRead()
{
	QByteArray data=file->readAll();
	if(!data.isEmpty())parser.pushData(QString::fromUtf8(data));
}

void ARpcTtyDevice::tryOpen()
{
	fd=open(ttyPath.toUtf8().constData(),O_RDWR);
	if(fd==-1)return;
	parser.reset();
	fcntl(fd,F_SETFL,O_NONBLOCK);
	file=new QFile(this);
	file->open(fd,QIODevice::ReadWrite|QIODevice::Unbuffered);
	notif=new QSocketNotifier(fd,QSocketNotifier::Read,this);
	connect(notif,SIGNAL(activated(int)),this,SLOT(onReadyRead()));
	connected=true;
	emit ttyConnected();
	QByteArray data=file->readAll();
	if(!data.isEmpty())parser.pushData(data);
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
