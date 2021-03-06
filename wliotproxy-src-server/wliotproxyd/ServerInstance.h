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

#ifndef SERVERINSTANCE_H
#define SERVERINSTANCE_H

#include "Devices.h"
#include "CmdArgParser.h"
#include "UnixControlSocket.h"
#include "TcpControlSocket.h"
#include "wliot/storages/FSStoragesDatabase.h"
#include "IExternCommandSource.h"
#include "JSProcessing/JSScriptsManager.h"
#include "VDILProcessing/VDILProgramsManager.h"
#include "wliot/FSDevicesNamesDatabase.h"
#include <QLocalServer>
#include <QLocalSocket>
//#include <libusb.h>

class ServerInstance
	:public QObject
{
	Q_OBJECT

private:
	ServerInstance();
	ServerInstance(const ServerInstance &t);
	ServerInstance& operator=(const ServerInstance &t);
	~ServerInstance();

public:
	static ServerInstance& inst();
	void setup(int argc,char **argv);
	void terminate();
	WLIOT::FSStoragesDatabase* storages();
	WLIOT::FSDevicesNamesDatabase* devNames();
	Devices* devices();
	JSScriptsManager* jsScripts();
	VDILProgramsManager* vdilPrograms();
	DataCollectionUnit* collectionUnit(const QUuid &deviceId,const QByteArray &sensorName);
	QUuid findDevId(const QByteArray &devIdOrName);
	QString srvBinDir();
//	libusb_context* usbContext();
//	QString externalServiceConfigurationDir(const )

private slots:
	void onStorageCreated(const WLIOT::StorageId &id);
	void onStorageRemoved(const WLIOT::StorageId &id);
	void onDeviceIdentified(QUuid id,QByteArray name);
//	void onDeviceConnected(QUuid id);
	void onDeviceDisconnected(QUuid id);

private:
	bool setUserAndGroup();
	void checkDataCollectionUnit(WLIOT::RealDevice *dev,const WLIOT::SensorDef &s);

public:
	bool terminated;

private:
	QString serverBinaryDir;
	bool ready;
	WLIOT::WLIOTProtocolDefs cfg;
	CmdArgParser cmdParser;
	QMap<QString,IExternCommandSource*> extCommands;
	UnixControlSocket localControl;
	TcpControlSocket remoteControl;
	QMap<QUuid,QMap<QByteArray,DataCollectionUnit*>> collectionUnits;
	WLIOT::FSStoragesDatabase *sensorsDb;
	Devices *mDevices;
	JSScriptsManager *jsScriptMgr;
	VDILProgramsManager *vdilProgramsMgr;
	WLIOT::FSDevicesNamesDatabase *devNamesDb;
//	libusb_context *usbCtx;
};

#endif // SERVERINSTANCE_H
