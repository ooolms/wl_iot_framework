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

#include "ServerInstance.h"
#include "MainServerConfig.h"
#include "UdpDataExport.h"
#include "wliot/devices/CommandCall.h"
#include "SysLogWrapper.h"
#include "ExternServices/AlterozoomAuthentificationStorage.h"
#include "ExternServices/IotkitAgentCommandSource.h"
#include "StdQFile.h"
#include <sys/syslog.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <pwd.h>
#include <grp.h>
#include <QFile>
#include <QCoreApplication>
#include <QThread>
#include <QSerialPortInfo>
#include <QDir>
#include <QSettings>

static QtMessageHandler oldHandler=0;
static const QRegExp uuidRegExp=QRegExp(
	"^\\{[0-9A-Fa-f]{8}\\-[0-9A-Fa-f]{4}\\-[0-9A-Fa-f]{4}\\-"
	"[0-9A-Fa-f]{4}\\-[0-9A-Fa-f]{12}\\}$");
static bool dupLogOutput=false;
static const QString cfgDir=QString("/etc/");
static const QString daemonVarDir=QString("/var/lib/wliotproxyd");

static void sigHandler(int sig)
{
	Q_UNUSED(sig)
	ServerInstance::inst().terminated=true;
	qApp->quit();
}

void stdoutMessageHandler(QtMsgType type,const QMessageLogContext &,const QString &str)
{
	if(type==QtDebugMsg)
	{
		StdQFile::stdout()->write(str.toUtf8());
		StdQFile::stdout()->write("\n");
		StdQFile::stdout()->flush();
	}
#if (QT_VERSION>=QT_VERSION_CHECK(5,5,0))
	else if(type==QtInfoMsg)
	{
		StdQFile::stdout()->write(str.toUtf8());
		StdQFile::stdout()->write("\n");
		StdQFile::stdout()->flush();
	}
#endif
	else
	{
		StdQFile::stderr()->write(str.toUtf8());
		StdQFile::stderr()->write("\n");
		StdQFile::stderr()->flush();
	}
}

void syslogMsgHandler(QtMsgType type,const QMessageLogContext &ctx,const QString &str)
{
	if(type==QtDebugMsg)
		SysLogWrapper::write(LOG_DEBUG,str);
#if (QT_VERSION>=QT_VERSION_CHECK(5,5,0))
	else if(type==QtInfoMsg)
		SysLogWrapper::write(LOG_INFO,str);
#endif
	else if(type==QtWarningMsg)
		SysLogWrapper::write(LOG_WARNING,str);
	else if(type==QtCriticalMsg)
		SysLogWrapper::write(LOG_ERR,str);
	else if(type==QtFatalMsg)
		SysLogWrapper::write(LOG_CRIT,str);
	if(dupLogOutput)
		stdoutMessageHandler(type,ctx,str);
}

ServerInstance::ServerInstance()
	:cmdParser(QStringList())
{
	ready=false;
	terminated=false;
	jsScriptMgr=0;
	gdilProgramsMgr=0;
	sensorsDb=new FSStoragesDatabase(this);
	connect(sensorsDb,&FSStoragesDatabase::storageCreated,this,&ServerInstance::onStorageCreated);
	connect(sensorsDb,&FSStoragesDatabase::storageRemoved,this,&ServerInstance::onStorageRemoved);
	extCommands["iotkit-agent"]=new IotkitAgentCommandSource;
	mDevices=new Devices(this);
	connect(mDevices,&Devices::deviceIdentified,this,&ServerInstance::onDeviceIdentified);
	connect(mDevices,&Devices::deviceDisconnected,this,&ServerInstance::onDeviceDisconnected);
	qsrand(QDateTime::currentMSecsSinceEpoch()%(qint64)std::numeric_limits<int>::max());
	devNamesDb=new FSDevicesNamesDatabase(this);
	connect(mDevices,&Devices::deviceIdentified,devNamesDb,&FSDevicesNamesDatabase::onDeviceIdentified);
//	libusb_init(&usbCtx);
}

ServerInstance::~ServerInstance()
{
//	libusb_exit(usbCtx);
}

ServerInstance& ServerInstance::inst()
{
	static ServerInstance instVar;
	return instVar;
}

void ServerInstance::setup(int argc,char **argv)
{
	if(ready)
		return;
	//	HidApiWrapper::inst().enumerate();
	cmdParser=CmdArgParser(argc,argv);
	dupLogOutput=cmdParser.keys.contains("v");
	oldHandler=qInstallMessageHandler(syslogMsgHandler);
	if(!MainServerConfig::readConfig(cmdParser))
		qFatal("Can't read server config: %s",(cfgDir.toUtf8()+"/wliotproxyd.ini").constData());
	setUserAndGroup();
	signal(SIGHUP,&sigHandler);
	signal(SIGINT,&sigHandler);
	signal(SIGQUIT,&sigHandler);
	signal(SIGILL,&sigHandler);
	signal(SIGABRT,&sigHandler);
	//	signal(SIGSEGV,&sigHandler);
	signal(SIGPIPE,&sigHandler);
	//	signal(SIGTERM,&sigHandler);
	AlterozoomAuthentificationStorage::readConfig("/var/lib/wliotproxyd/alterozoom_auth.xml");
	UdpDataExport::setExportAddress(MainServerConfig::dataUdpExportAddress);
	QDir dbDir(daemonVarDir);
	dbDir.mkdir("sensors_database");
	if(!dbDir.exists()||!dbDir.exists("sensors_database"))
		qFatal("Daemon directory %s does not exists",daemonVarDir.toUtf8().constData());
	devNamesDb->initDb(daemonVarDir+"/devnames.xml");
	sensorsDb->open(daemonVarDir+"/sensors_database");
	QList<StorageId> ids;
	for(StorageId id:ids)
		devNamesDb->onNameFromStorage(id.deviceId,sensorsDb->existingStorage(id)->deviceName());
	if(cmdParser.keys.contains("d"))
	{
		if(fork())
			_exit(0);
		setsid();
		daemon(0,0);
	}
	if(!MainServerConfig::networkCrtChain.isEmpty()&&!MainServerConfig::networkKey.isNull())
	{
		qDebug()<<"Starting remote control via tcp";
		remoteControl.start(MainServerConfig::networkCrtChain,MainServerConfig::networkKey);
	}
	mDevices->setup();
	localControl.start();
	jsScriptMgr=new JSScriptsManager(this);
	gdilProgramsMgr=new GDILProgramsManager(this);
	ready=true;
}

FSStoragesDatabase* ServerInstance::storages()
{
	return sensorsDb;
}

FSDevicesNamesDatabase* ServerInstance::devNames()
{
	return devNamesDb;
}

void ServerInstance::terminate()
{
	qDebug()<<"Terminate server";
	for(auto m:collectionUnits)
		for(auto i:m)
			delete i;
	collectionUnits.clear();
	sensorsDb->close();
	mDevices->terminate();
	delete jsScriptMgr;
	jsScriptMgr=0;
	delete gdilProgramsMgr;
	gdilProgramsMgr=0;
}

Devices* ServerInstance::devices()
{
	return mDevices;
}

JSScriptsManager* ServerInstance::jsScripts()
{
	return jsScriptMgr;
}

GDILProgramsManager* ServerInstance::gdilPrograms()
{
	return gdilProgramsMgr;
}

void ServerInstance::onStorageCreated(const StorageId &id)
{
	RealDevice *dev=mDevices->deviceById(id.deviceId);
	if(!dev)
		return;
	QList<SensorDef> sensors;
	if(!dev->getSensorsDescription(sensors))
		return;
	for(SensorDef &s:sensors)
		if(s.name==id.sensorName)
		{
			checkDataCollectionUnit(dev,s);
			break;
		}
	devNamesDb->onNameFromStorage(id.deviceId,sensorsDb->existingStorage(id)->deviceName());
}

void ServerInstance::onStorageRemoved(const StorageId &id)
{
	if(collectionUnits.contains(id.deviceId)&&collectionUnits[id.deviceId].contains(id.sensorName))
	{
		collectionUnits[id.deviceId][id.sensorName]->deleteLater();
		collectionUnits[id.deviceId].remove(id.sensorName);
		if(collectionUnits[id.deviceId].isEmpty())
			collectionUnits.remove(id.deviceId);
	}
}

void ServerInstance::onDeviceIdentified(QUuid id,QByteArray name)
{
	Q_UNUSED(name)
	RealDevice *dev=devices()->deviceById(id);
	if(!dev)return;
	QList<SensorDef> sensors;
	if(dev->getSensorsDescription(sensors))
	{
		for(auto &s:sensors)
			checkDataCollectionUnit(dev,s);
	}
	else
	{
		QList<StorageId> ids;
		if(ServerInstance::inst().storages()->listStorages(ids))
		{
			for(auto &id:ids)
			{
				if(id.deviceId!=dev->id())continue;
				ISensorStorage *stor=ServerInstance::inst().storages()->existingStorage(id);
				SensorDef sens=stor->sensor();
				checkDataCollectionUnit(dev,sens);
			}
		}
	}
}

void ServerInstance::onDeviceDisconnected(QUuid id)
{
	if(id.isNull())return;
	if(!collectionUnits.contains(id))
		return;
	auto &units=collectionUnits[id];
	for(auto i=units.begin();i!=units.end();++i)
		i.value()->deleteLater();
	collectionUnits.remove(id);
}

void ServerInstance::setUserAndGroup()
{
	if(MainServerConfig::serverProcessUserName.isEmpty())
		return;
	struct passwd *userEnt=getpwnam(MainServerConfig::serverProcessUserName.toUtf8().constData());
	if(!userEnt)
		qFatal("No system user %s",MainServerConfig::serverProcessUserName.toUtf8().constData());
	struct group *grEnt=0;
	if(!MainServerConfig::serverProcessGroupName.isEmpty())
	{
		grEnt=getgrnam(MainServerConfig::serverProcessGroupName.toUtf8().constData());
		if(!grEnt)
			qFatal("No system group %s",MainServerConfig::serverProcessGroupName.toUtf8().constData());
	}
	if(grEnt)
	{
		if(setgid(grEnt->gr_gid))
			qFatal("Can't change group to %s",MainServerConfig::serverProcessGroupName.toUtf8().constData());
	}
	else
	{
		if(setgid(userEnt->pw_gid))
			qFatal("Can't change group to user's group for user %s",
				MainServerConfig::serverProcessUserName.toUtf8().constData());
	}
	if(setuid(userEnt->pw_uid))
		qFatal("Can't change user to %s",MainServerConfig::serverProcessUserName.toUtf8().constData());
}

void ServerInstance::checkDataCollectionUnit(RealDevice *dev,const SensorDef &s)
{
	QUuid devId=dev->id();
	if(collectionUnits.contains(devId)&&collectionUnits[devId].contains(s.name))
		return;
	StorageId stId={devId,s.name};
	if(!sensorsDb->hasStorage(stId))
		return;
	ISensorStorage *stor=sensorsDb->existingStorage(stId);
	if(!stor)
		return;
	if(!stor->isOpened()&&!stor->open())
		return;
	DataCollectionUnit *unit=new DataCollectionUnit(dev,stor,this);
	collectionUnits[devId][s.name]=unit;
	qDebug()<<"Data collection unit created: "<<dev->name()<<": "<<s.name;
	connect(unit,&DataCollectionUnit::infoMessage,[](const QByteArray &msg)
	{
		qDebug()<<msg;
	});
	connect(unit,&DataCollectionUnit::errorMessage,[](const QByteArray &msg)
	{
		qWarning()<<msg;
	});
}

DataCollectionUnit* ServerInstance::collectionUnit(const QUuid &deviceId,const QByteArray &sensorName)
{
	if(!collectionUnits.contains(deviceId))
		return 0;
	if(!collectionUnits[deviceId].contains(sensorName))
		return 0;
	return collectionUnits[deviceId][sensorName];
}

QUuid ServerInstance::findDevId(const QByteArray &devIdOrName)
{
	QUuid id(devIdOrName);
	if(!id.isNull())return id;
	return devNamesDb->findDevice(devIdOrName);
}

//libusb_context* ServerInstance::usbContext()
//{
//	return usbCtx;
//}
