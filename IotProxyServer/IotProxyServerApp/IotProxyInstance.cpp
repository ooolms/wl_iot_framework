#include "IotProxyInstance.h"
#include "IotProxyConfig.h"
#include "ARpcBase/ARpcUnsafeCall.h"
#include "ARpcBase/ARpcSyncCall.h"
#include "SysLogWrapper.h"
#include <sys/syslog.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <pwd.h>
#include <grp.h>
#include <QCoreApplication>
#include <QThread>
#include <QDir>
#include <QSettings>

static QtMessageHandler oldHandler=0;

static void sigHandler(int sig)
{
	Q_UNUSED(sig)
	IotProxyInstance::inst().terminateProxy=true;
	qApp->quit();
}

void msgHandler(QtMsgType type,const QMessageLogContext &ctx,const QString &str)
{
	if(type==QtDebugMsg)
		SysLogWrapper::write(LOG_DEBUG,str);
	else if(type==QtInfoMsg)
		SysLogWrapper::write(LOG_INFO,str);
	else if(type==QtWarningMsg)
		SysLogWrapper::write(LOG_WARNING,str);
	else if(type==QtCriticalMsg)
		SysLogWrapper::write(LOG_ERR,str);
	else if(type==QtFatalMsg)
		SysLogWrapper::write(LOG_CRIT,str);
	oldHandler(type,ctx,str);
}

IotProxyInstance::IotProxyInstance()
	:cmdParser(QStringList())
{
	ready=false;
	cfgDir="/etc";
}

IotProxyInstance::~IotProxyInstance()
{
}

IotProxyInstance &IotProxyInstance::inst()
{
	static IotProxyInstance instVar;
	return instVar;
}

void IotProxyInstance::setup(int argc,char **argv)
{
	if(ready)return;
	cmdParser=CmdArgParser(argc,argv);
	oldHandler=qInstallMessageHandler(msgHandler);
	if(!IotProxyConfig::readConfig(cfgDir,cmdParser))
	{
		qFatal("Can't read server config");
		return;
	}
	if(cmdParser.isKeySet("d"))
	{
		if(fork())
		{
			_exit(0);
			return;
		}
		setsid();
		daemon(0,0);
	}
	setUserAndGroup();
	signal(SIGINT,&sigHandler);
	signal(SIGABRT,&sigHandler);
	signal(SIGQUIT,&sigHandler);
	signal(SIGTERM,&sigHandler);
	setupControllers();
	ready=true;
}

ARpcTtyDevice* IotProxyInstance::findTtyDevByPortName(const QString &portName)
{
	for(auto &dev:ttyDevices)
		if(dev->portName()==portName)return dev;
	return 0;
}

void IotProxyInstance::devMsgHandler(const ARpcMessage &m)
{
	ARpcDevice *dev=qobject_cast<ARpcDevice*>(sender());
	if(!dev)return;
	if(m.title==ARpcConfig::infoMsg)
		qInfo()<<"Device info message ("<<dev->id()<<":"<<dev->name()<<")"<<m.args.join("|");
}

void IotProxyInstance::setupControllers()
{
	for(QString &addr:IotProxyConfig::tcpDevices)
	{
		if(addr.isEmpty())continue;
		ARpcTcpDevice *dev=new ARpcTcpDevice(QHostAddress(addr),this);
		if(!dev->isConnected()||!dev->identify())
		{
			delete dev;
			continue;
		}
		tcpDevices[dev->id()]=dev;
		qInfo()<<"Tcp device found: "<<dev->id()<<"; "<<dev->name();
		QObject::connect(dev,&ARpcTtyDevice::rawMessage,this,&IotProxyInstance::devMsgHandler);
	}
	for(QString &filePath:IotProxyConfig::ttyDevices)
	{
		if(filePath.isEmpty())continue;
		ARpcTtyDevice *dev=new ARpcTtyDevice(filePath,this);
		if(!dev->isConnected()||!dev->identify())
		{
			delete dev;
			continue;
		}
		ttyDevices[dev->id()]=dev;
		qInfo()<<"Tty device found: "<<dev->id()<<"; "<<dev->name();
		QObject::connect(dev,&ARpcTtyDevice::rawMessage,this,&IotProxyInstance::devMsgHandler);
	}
}

void IotProxyInstance::setUserAndGroup()
{
	if(IotProxyConfig::serverProcessUserName.isEmpty())return;
	struct passwd *userEnt=getpwnam(IotProxyConfig::serverProcessUserName.toUtf8().constData());
	if(!userEnt)
	{
		qFatal("No system user "+IotProxyConfig::serverProcessUserName.toUtf8());
		return;
	}
	struct group *grEnt=0;
	if(!IotProxyConfig::serverProcessGroupName.isEmpty())
	{
		grEnt=getgrnam(IotProxyConfig::serverProcessGroupName.toUtf8().constData());
		if(!grEnt)
		{
			qFatal("No system group "+IotProxyConfig::serverProcessGroupName.toUtf8());
			return;
		}
	}
	if(setuid(userEnt->pw_uid))
	{
		qFatal("Can't change user to "+IotProxyConfig::serverProcessUserName.toUtf8());
		return;
	}
	if(grEnt)
	{
		if(setgid(grEnt->gr_gid))
		{
			qFatal("Can't change group to "+IotProxyConfig::serverProcessGroupName.toUtf8());
			return;
		}
	}
	else
	{
		if(setgid(userEnt->pw_gid))
		{
			qFatal("Can't change group to user group for user "+IotProxyConfig::serverProcessUserName.toUtf8());
			return;
		}
	}
}
