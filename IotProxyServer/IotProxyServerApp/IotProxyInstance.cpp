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
#include <QSerialPortInfo>
#include <QDir>
#include <QSettings>

//TODO collect data

static QtMessageHandler oldHandler=0;
static const QRegExp uuidRegExp=QRegExp("^\\{[0-9A-Fa-f]{8}\\-[0-9A-Fa-f]{4}\\-[0-9A-Fa-f]{4}\\-"
	"[0-9A-Fa-f]{4}\\-[0-9A-Fa-f]{12}\\}$");

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
	daemonVarDir="/var/lib/WLIotProxyServer";
	sensorsDb=new ARpcLocalDatabase(this);
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
		qFatal("Can't read server config: "+cfgDir.toUtf8()+"/WLIotProxyServer.ini");
		return;
	}
	QDir dbDir(daemonVarDir);
	dbDir.mkdir("sensors_database");
	if(!dbDir.exists()||!dbDir.exists("sensors_database"))
	{
		qFatal("Daemon directory "+daemonVarDir.toUtf8()+" does not exists");
		return;
	}
	sensorsDb->open(daemonVarDir+"/sensors_database");
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
	for(ARpcTtyDevice *dev:allTtyDevices)
		if(dev->portName()==portName)return dev;
	return 0;
}

ARpcTcpDevice* IotProxyInstance::findTcpDevByAddress(const QHostAddress &address)
{
	for(ARpcTcpDevice *dev:allTcpDevices)
		if(dev->getAddress()==address)return dev;
	return 0;
}

ARpcDevice* IotProxyInstance::deviceById(const QUuid &id)
{
	if(identifiedTtyDevices.contains(id))
		return identifiedTtyDevices[id];
	else if(identifiedTcpDevices.contains(id))
		return identifiedTcpDevices[id];
	return 0;
}

ARpcDevice* IotProxyInstance::deviceByName(const QString &name)
{
	for(ARpcTtyDevice *dev:identifiedTtyDevices)
		if(dev->name()==name)return dev;
	for(ARpcTcpDevice *dev:identifiedTcpDevices)
		if(dev->name()==name)return dev;
	return 0;
}

ARpcDevice* IotProxyInstance::deviceByIdOrName(const QString str)
{
	if(str.isEmpty())return 0;
	QUuid id(str);
	if(id.isNull())return deviceByName(str);
	else return deviceById(id);
}

ARpcLocalDatabase* IotProxyInstance::getSensorsDb()
{
	return sensorsDb;
}

void IotProxyInstance::devMsgHandler(const ARpcMessage &m)
{
	ARpcDevice *dev=qobject_cast<ARpcDevice*>(sender());
	if(!dev)return;
	if(m.title==ARpcConfig::infoMsg)
		qDebug()<<"Device info message ("<<dev->id()<<":"<<dev->name()<<")"<<m.args.join("|");
}

void IotProxyInstance::onTtyDeviceIdentified()
{
	ARpcTtyDevice *dev=(ARpcTtyDevice*)sender();
	for(auto i=identifiedTtyDevices.begin();i!=identifiedTtyDevices.end();++i)
	{
		if(i.value()==dev)
		{
			identifiedTtyDevices.erase(i);
			break;
		}
	}
	if(dev->isIdentified())
		identifiedTtyDevices[dev->id()]=dev;
}

void IotProxyInstance::onTcpDeviceIdentified()
{
	ARpcTcpDevice *dev=(ARpcTcpDevice*)sender();
	for(auto i=identifiedTcpDevices.begin();i!=identifiedTcpDevices.end();++i)
	{
		if(i.value()==dev)
		{
			identifiedTcpDevices.erase(i);
			break;
		}
	}
	if(dev->isIdentified())
		identifiedTcpDevices[dev->id()]=dev;
}

void IotProxyInstance::onTtyDeviceDisconnected()
{
	ARpcTtyDevice *dev=(ARpcTtyDevice*)sender();
	for(auto i=identifiedTtyDevices.begin();i!=identifiedTtyDevices.end();++i)
	{
		if(i.value()==dev)
		{
			identifiedTtyDevices.erase(i);
			break;
		}
	}
}

void IotProxyInstance::onTcpDeviceDisconnected()
{
	ARpcTcpDevice *dev=(ARpcTcpDevice*)sender();
	for(auto i=identifiedTcpDevices.begin();i!=identifiedTcpDevices.end();++i)
	{
		if(i.value()==dev)
		{
			identifiedTcpDevices.erase(i);
			break;
		}
	}
}

void IotProxyInstance::setupControllers()
{
	for(QString &addr:IotProxyConfig::tcpAddresses)
	{
		if(addr.isEmpty())continue;
		ARpcTcpDevice *dev=new ARpcTcpDevice(QHostAddress(addr),this);
		allTcpDevices.append(dev);
		if(dev->isConnected()&&dev->identify())
		{
			identifiedTcpDevices[dev->id()]=dev;
			//TODO collect data
		}
		connect(dev,&ARpcTtyDevice::rawMessage,this,&IotProxyInstance::devMsgHandler);
		connect(dev,&ARpcTcpDevice::identified,this,&IotProxyInstance::onTcpDeviceIdentified);
		connect(dev,&ARpcTcpDevice::disconnected,this,&IotProxyInstance::onTcpDeviceDisconnected);
	}
	QStringList ttyPorts=extractTtyPorts();
	for(QString &portName:ttyPorts)
	{
		if(portName.isEmpty())continue;
		ARpcTtyDevice *dev=new ARpcTtyDevice(portName,this);
		allTtyDevices.append(dev);
		if(dev->isConnected()&&dev->identify())
		{
			identifiedTtyDevices[dev->id()]=dev;
			//TODO collect data
		}
		connect(dev,&ARpcTtyDevice::rawMessage,this,&IotProxyInstance::devMsgHandler);
		connect(dev,&ARpcTcpDevice::identified,this,&IotProxyInstance::onTtyDeviceIdentified);
		connect(dev,&ARpcTcpDevice::disconnected,this,&IotProxyInstance::onTtyDeviceDisconnected);
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

QStringList IotProxyInstance::extractTtyPorts()
{
	QSet<QString> ports=IotProxyConfig::ttyPortNames.toSet();
	QList<QSerialPortInfo> devs=QSerialPortInfo::availablePorts();
	QList<QRegExp> manufExps;
	for(QString &e:IotProxyConfig::ttyManufacturerRegExps)
		manufExps.append(QRegExp(e,Qt::CaseSensitive,QRegExp::WildcardUnix));
	for(QSerialPortInfo &info:devs)
	{
		bool found=false;
		for(QRegExp &e:manufExps)
		{
			if(e.indexIn(info.manufacturer())!=-1)
			{
				found=true;
				break;
			}
		}
		if(!found)continue;
		ports.insert(info.portName());
	}
	return ports.toList();
}
