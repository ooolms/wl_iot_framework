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
#include "IotProxyRemoteControlSocket.h"
#include "ARpcLocalStorage/ARpcLocalDatabase.h"
#include "DataCollectionUnit.h"
#include "LsTtyUsbDevices.h"
#include "IExternCommandSource.h"
#include "ARpcDevices/ARpcTcpDeviceDetect.h"
#include "JSDataProcessing/JSThread.h"
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

public slots:
	void setupControllers();

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
	void onNewTcpDeviceConnected(QTcpSocket *sock,bool &accepted);

private:
	void setUserAndGroup();
	QStringList extractTtyPorts();
	void deviceIdentified(ARpcRealDevice *dev);
	void checkDataCollectionUnit(ARpcRealDevice *dev,const ARpcSensor &s,const DeviceAndSensorId &stId);
	void loadDataProcessingScripts();

	template<typename T,typename=std::enable_if<std::is_base_of<ARpcRealDevice,T>::value>>
	ARpcRealDevice* findDevById(const QUuid &id,QList<T*> &list)
	{
		static_assert(std::is_base_of<ARpcRealDevice,T>::value,"Invalid template argument");
		for(ARpcRealDevice *d:list)
			if(d->id()==id)return d;
		return 0;
	}

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
	IotProxyControlSocket localControl;
	IotProxyRemoteControlSocket remoteControl;
	ARpcLocalDatabase *sensorsDb;
	QFileSystemWatcher watcher;
	QList<LsTtyUsbDevices::DeviceInfo> allTtyUsbDevices;
	ARpcTcpDeviceDetect tcpServer;
	QList<JSThread*> jsThreads;
};

#endif // IOTPROXYINSTANCE_H
