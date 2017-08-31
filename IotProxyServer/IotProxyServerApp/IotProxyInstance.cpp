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

#include "IotProxyInstance.h"
#include "IotProxyConfig.h"
#include "UdpDataExport.h"
#include "ARpcBase/ARpcUnsafeCall.h"
#include "ARpcBase/ARpcSyncCall.h"
#include "SysLogWrapper.h"
#include "ExternServices/IotkitAgentCommandSource.h"
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

static QtMessageHandler oldHandler=0;
static const QRegExp uuidRegExp=QRegExp("^\\{[0-9A-Fa-f]{8}\\-[0-9A-Fa-f]{4}\\-[0-9A-Fa-f]{4}\\-"
	"[0-9A-Fa-f]{4}\\-[0-9A-Fa-f]{12}\\}$");
static bool dupLogOutput=false;
static const QString cfgDir=QString("/etc/");
static const QString daemonVarDir=QString("/var/lib/wliotproxyd");

static void sigHandler(int sig)
{
	Q_UNUSED(sig)
	IotProxyInstance::inst().terminated=true;
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

//CRIT если подключенный клиент помрет в процессе выполнения команды, сервер падает (убивается CommandProcessor?)

IotProxyInstance::IotProxyInstance()
	:cmdParser(QStringList())
{
	ready=false;
	terminated=false;
	sensorsDb=new ARpcLocalDatabase(this);
	connect(sensorsDb,&ARpcLocalDatabase::storageCreated,this,&IotProxyInstance::onStorageCreated);
	connect(sensorsDb,&ARpcLocalDatabase::storageRemoved,this,&IotProxyInstance::onStorageRemoved);
	extCommands["iotkit-agent"]=new IotkitAgentCommandSource;
	watcher.addPath("/dev/");
	connect(&watcher,&QFileSystemWatcher::directoryChanged,this,&IotProxyInstance::setupControllers);
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
	if(!IotProxyConfig::readConfig(cmdParser))
	{
		qFatal("Can't read server config: "+cfgDir.toUtf8()+"/wliotproxyd.ini");
		return;
	}
	UdpDataExport::setExportAddress(IotProxyConfig::dataUdpExportAddress);
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
//	signal(SIGSEGV,&sigHandler);
	signal(SIGPIPE,&sigHandler);
//	signal(SIGTERM,&sigHandler);
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
		if(dev->address()==address)return dev;
	return 0;
}

ARpcRealDevice* IotProxyInstance::deviceById(const QUuid &id)
{
	if(identifiedDevices.contains(id))
		return identifiedDevices[id];
	return 0;
}

ARpcRealDevice* IotProxyInstance::deviceByName(const QString &name)
{
	for(ARpcRealDevice *dev:identifiedDevices)
		if(dev->name()==name)return dev;
	return 0;
}

ARpcRealDevice* IotProxyInstance::deviceByIdOrName(const QString str)
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

DataCollectionUnit *IotProxyInstance::collectionUnit(const DeviceAndSensorId &id)
{
	if(!collectionUnits.contains(id.deviceId))return 0;
	if(!collectionUnits[id.deviceId].contains(id.sensorName))return 0;
	return collectionUnits[id.deviceId][id.sensorName];
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
	collectionUnits.clear();
	sensorsDb->close();
	for(auto d:allTcpDevices)
		delete d;
	allTcpDevices.clear();
	for(auto d:allTtyDevices)
		delete d;
	allTtyDevices.clear();
}

const QList<ARpcTtyDevice*>& IotProxyInstance::ttyDevices()
{
	return allTtyDevices;
}

const QList<ARpcTcpDevice*>& IotProxyInstance::tcpDevices()
{
	return allTcpDevices;
}

void IotProxyInstance::devMsgHandler(const ARpcMessage &m)
{
	ARpcRealDevice *dev=qobject_cast<ARpcRealDevice*>(sender());
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
	ARpcRealDevice *dev=deviceById(id.deviceId);
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
		QHostAddress hAddr(addr);
		if(findTcpDevByAddress(hAddr))continue;
		ARpcTcpDevice *dev=new ARpcTcpDevice(hAddr,this);
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
		if(findTtyDevByPortName(portName))continue;
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
	QSet<QString> ports;
	QList<QRegExp> portNameRegExps;
	for(QString &pName:IotProxyConfig::ttyPortNames)
		portNameRegExps.append(QRegExp(pName,Qt::CaseSensitive,QRegExp::WildcardUnix));
	QList<LsTtyUsbDevices::DeviceInfo> ttyDevs=LsTtyUsbDevices::allTtyUsbDevices();
	for(auto &dev:ttyDevs)
	{
		bool found=false;
		for(const QRegExp &exp:portNameRegExps)
		{
			if(exp.exactMatch(dev.ttyPortName))
			{
				found=true;
				ports.insert(dev.ttyPortName);
			}
		}
		if(found)continue;
		for(auto &vpItem:IotProxyConfig::ttyByVidPid)
		{
			bool dontMatch=false;
			if(vpItem.vid!=dev.vendorId)
				dontMatch=true;
			else if(!vpItem.pid.isEmpty()&&vpItem.pid!=dev.productId)
				dontMatch=true;
			if(!dontMatch)
			{
				ports.insert(dev.ttyPortName);
				break;
			}
		}
	}
	return ports.toList();
}

void IotProxyInstance::deviceIdentified(ARpcRealDevice *dev)
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

void IotProxyInstance::checkDataCollectionUnit(ARpcRealDevice *dev,const ARpcSensor &s,const DeviceAndSensorId &stId)
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
