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

#ifndef IOTPROXYINSTANCE_H
#define IOTPROXYINSTANCE_H

#include "ARpcDevices/ARpcTtyDevice.h"
#include "ARpcDevices/ARpcTcpDevice.h"
#include "CmdArgParser.h"
#include "IotProxyControlSocket.h"
#include "ARpcLocalStorage/ARpcLocalDatabase.h"
#include "DataCollectionUnit.h"
#include "LsTtyUsbDevices.h"
#include "IExternCommandSource.h"
#include <QLocalServer>
#include <QLocalSocket>

class IotProxyInstance
	:public QObject
{
	Q_OBJECT
private:
	IotProxyInstance();
	IotProxyInstance(const IotProxyInstance &t);
	IotProxyInstance& operator=(const IotProxyInstance &t);
	~IotProxyInstance();

public:
	static IotProxyInstance& inst();
	void setup(int argc,char **argv);
	ARpcTtyDevice* findTtyDevByPortName(const QString &portName);
	ARpcTcpDevice* findTcpDevByAddress(const QHostAddress &address);
	ARpcRealDevice* deviceById(const QUuid &id);
	ARpcRealDevice* deviceByName(const QString &name);
	ARpcRealDevice* deviceByIdOrName(const QString str);
	ARpcLocalDatabase* getSensorsDb();
	DataCollectionUnit* collectionUnit(const DeviceAndSensorId &id);
	bool findUsbTtyDeviceByPortName(const QString &portName,LsTtyUsbDevices::DeviceInfo &info);
	void terminate();
	const QList<ARpcTtyDevice*>& ttyDevices();
	const QList<ARpcTcpDevice*>& tcpDevices();

private slots:
	void devMsgHandler(const ARpcMessage &m);
	void onTtyDeviceIdentified();
	void onTcpDeviceIdentified();
	void onTtyDeviceDisconnected();
	void onTcpDeviceDisconnected();
	void onStorageCreated(const DeviceAndSensorId &id);
	void onStorageRemoved(const DeviceAndSensorId &id);
	void setupControllers();

private:
	void setUserAndGroup();
	QStringList extractTtyPorts();
	void deviceIdentified(ARpcRealDevice *dev);
	void checkDataCollectionUnit(ARpcRealDevice *dev,const ARpcSensor &s,const DeviceAndSensorId &stId);

public:
	bool terminated;

private:
	bool ready;
	ARpcConfig cfg;
	CmdArgParser cmdParser;
	QList<ARpcTtyDevice*> allTtyDevices;
	QList<ARpcTcpDevice*> allTcpDevices;
	QMap<QUuid,ARpcRealDevice*> identifiedDevices;
	QMap<QUuid,QMap<QString,DataCollectionUnit*>> collectionUnits;
	QMap<QString,IExternCommandSource*> extCommands;
	QString cfgDir;
	QString daemonVarDir;
	IotProxyControlSocket ctlSocket;
	ARpcLocalDatabase *sensorsDb;
	QFileSystemWatcher watcher;
	QList<LsTtyUsbDevices::DeviceInfo> allTtyUsbDevices;
};

#endif // IOTPROXYINSTANCE_H
