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
//TODO use allTtyUsbDevices to select devices by vendor id and product id, show this info in list_tty command

static QtMessageHandler oldHandler=0;
static const QRegExp uuidRegExp=QRegExp("^\\{[0-9A-Fa-f]{8}\\-[0-9A-Fa-f]{4}\\-[0-9A-Fa-f]{4}\\-"
	"[0-9A-Fa-f]{4}\\-[0-9A-Fa-f]{12}\\}$");
static bool dupLogOutput=false;

static void sigHandler(int sig)
{
	Q_UNUSED(sig)
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
	if(dupLogOutput)oldHandler(type,ctx,str);
}

IotProxyInstance::IotProxyInstance()
	:cmdParser(QStringList())
{
	ready=false;
	cfgDir="/etc";
	daemonVarDir="/var/lib/wliotproxyd";
	sensorsDb=new ARpcLocalDatabase(this);
	connect(sensorsDb,&ARpcLocalDatabase::storageCreated,this,&IotProxyInstance::onStorageCreated);
	connect(sensorsDb,&ARpcLocalDatabase::storageRemoved,this,&IotProxyInstance::onStorageRemoved);
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
//	HidApiWrapper::inst().enumerate();
	allTtyUsbDevices=LsTtyUsbDevices::allTtyUsbDevices();
	cmdParser=CmdArgParser(argc,argv);
	dupLogOutput=cmdParser.isKeySet("v");
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
	signal(SIGHUP,&sigHandler);
	signal(SIGINT,&sigHandler);
	signal(SIGQUIT,&sigHandler);
	signal(SIGILL,&sigHandler);
	signal(SIGABRT,&sigHandler);
	signal(SIGSEGV,&sigHandler);
	signal(SIGPIPE,&sigHandler);
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
	if(identifiedDevices.contains(id))
		return identifiedDevices[id];
	return 0;
}

ARpcDevice* IotProxyInstance::deviceByName(const QString &name)
{
	for(ARpcDevice *dev:identifiedDevices)
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

bool IotProxyInstance::findUsbTtyDeviceByPortName(const QString &portName, LsTtyUsbDevices::DeviceInfo &info)
{
	for(auto &i:allTtyUsbDevices)
		if(i.ttyPortName==portName)
		{
			info=i;
			return true;
		}
	return false;
}

void IotProxyInstance::terminate()
{
	qDebug()<<"Terminate server";
	for(auto m:collectionUnits)
		for(auto i:m)
			delete i;
	sensorsDb->close();
	for(auto d:allTcpDevices)
		delete d;
	for(auto d:allTtyDevices)
		delete d;
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
	deviceIdentified(dev);
}

void IotProxyInstance::onTcpDeviceIdentified()
{
	ARpcTcpDevice *dev=(ARpcTcpDevice*)sender();
	deviceIdentified(dev);
}

void IotProxyInstance::onTtyDeviceDisconnected()
{
	ARpcTtyDevice *dev=(ARpcTtyDevice*)sender();
	for(auto i=identifiedDevices.begin();i!=identifiedDevices.end();++i)
	{
		if(i.value()==dev)
		{
			qDebug()<<"Tty device disconnected: "<<i.key();
			identifiedDevices.erase(i);
			break;
		}
	}
	if(dev->isIdentified())
	{
		QUuid id=dev->id();
		if(!collectionUnits.contains(id))return;
		for(auto i=collectionUnits[id].begin();i!=collectionUnits[id].end();++i)
			delete i.value();
		collectionUnits.remove(id);
	}
}

void IotProxyInstance::onTcpDeviceDisconnected()
{
	ARpcTcpDevice *dev=(ARpcTcpDevice*)sender();
	for(auto i=identifiedDevices.begin();i!=identifiedDevices.end();++i)
	{
		if(i.value()==dev)
		{
			qDebug()<<"Tcp device disconnected: "<<i.key();
			identifiedDevices.erase(i);
			break;
		}
	}
	if(dev->isIdentified())
	{
		QUuid id=dev->id();
		if(!collectionUnits.contains(id))return;
		for(auto i=collectionUnits[id].begin();i!=collectionUnits[id].end();++i)
			delete i.value();
		collectionUnits.remove(id);
	}
}

void IotProxyInstance::onStorageCreated(const DeviceAndSensorId &id)
{
	ARpcDevice *dev=deviceById(id.deviceId);
	if(!dev)return;
	QList<ARpcSensor> sensors;
	if(!dev->getSensorsDescription(sensors))return;
	for(ARpcSensor &s:sensors)
		if(s.name==id.sensorName)
		{
			checkDataCollectionUnit(dev,s,id);
			break;
		}
}

void IotProxyInstance::onStorageRemoved(const DeviceAndSensorId &id)
{
	if(collectionUnits.contains(id.deviceId)&&collectionUnits[id.deviceId].contains(id.sensorName))
	{
		delete collectionUnits[id.deviceId][id.sensorName];
		collectionUnits[id.deviceId].remove(id.sensorName);
		if(collectionUnits[id.deviceId].isEmpty())
			collectionUnits.remove(id.deviceId);
	}
}

void IotProxyInstance::setupControllers()
{
	qDebug()<<"Tcp devices from config: "<<IotProxyConfig::tcpAddresses;
	for(QString &addr:IotProxyConfig::tcpAddresses)
	{
		if(addr.isEmpty())continue;
		ARpcTcpDevice *dev=new ARpcTcpDevice(QHostAddress(addr),this);
		allTcpDevices.append(dev);
		if(dev->isConnected()&&dev->identify())
			deviceIdentified(dev);
		connect(dev,&ARpcTtyDevice::rawMessage,this,&IotProxyInstance::devMsgHandler);
		connect(dev,&ARpcTcpDevice::identificationChanged,this,&IotProxyInstance::onTcpDeviceIdentified);
		connect(dev,&ARpcTcpDevice::disconnected,this,&IotProxyInstance::onTcpDeviceDisconnected);
	}
	QStringList ttyPorts=extractTtyPorts();
	qDebug()<<"Tty devices from config: "<<ttyPorts;
	for(QString &portName:ttyPorts)
	{
		if(portName.isEmpty())continue;
		ARpcTtyDevice *dev=new ARpcTtyDevice(portName,this);
		allTtyDevices.append(dev);
		if(dev->isConnected()&&dev->identify())
			deviceIdentified(dev);
		connect(dev,&ARpcTtyDevice::rawMessage,this,&IotProxyInstance::devMsgHandler);
		connect(dev,&ARpcTcpDevice::identificationChanged,this,&IotProxyInstance::onTtyDeviceIdentified);
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

void IotProxyInstance::deviceIdentified(ARpcDevice *dev)
{
	for(auto i=identifiedDevices.begin();i!=identifiedDevices.end();++i)
	{
		if(i.value()==dev)
		{
			identifiedDevices.erase(i);
			break;
		}
	}
	if(!dev->isIdentified())return;
	identifiedDevices[dev->id()]=dev;
	qDebug()<<"Device identified: "<<dev->name()<<":"<<dev->id();
	QList<ARpcSensor> sensors;
	if(!dev->getSensorsDescription(sensors))return;
	for(auto &s:sensors)
	{
		DeviceAndSensorId stId={dev->id(),s.name};
		checkDataCollectionUnit(dev,s,stId);
	}
}

void IotProxyInstance::checkDataCollectionUnit(ARpcDevice *dev,const ARpcSensor &s,const DeviceAndSensorId &stId)
{
	QUuid devId=dev->id();
	if(collectionUnits.contains(devId)&&collectionUnits[devId].contains(s.name))return;
	if(!sensorsDb->hasStorage(stId))return;
	ARpcISensorStorage *stor=sensorsDb->existingStorage(stId);
	if(!stor)return;
	if(!stor->isOpened()&&!stor->open())return;
	DataCollectionUnit *unit=new DataCollectionUnit(dev,stor,s,this);
	collectionUnits[devId][s.name]=unit;
	qDebug()<<"Data collection unit created: "<<dev->name()<<": "<<s.name;
//	connect(unit,&DataCollectionUnit::infoMessage,logView,&LogView::writeInfo);
//	connect(unit,&DataCollectionUnit::errorMessage,logView,&LogView::writeError);
}
