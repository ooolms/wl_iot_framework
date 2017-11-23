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

#include "IotProxyDevices.h"
#include "CmdArgParser.h"
#include "IotProxyControlSocket.h"
#include "IotProxyRemoteControlSocket.h"
#include "ARpcLocalStorage/ARpcLocalDatabase.h"
#include "IExternCommandSource.h"
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

public:
	static IotProxyInstance& inst();
	void setup(int argc,char **argv);
	void terminate();
	ARpcLocalDatabase* sensorsStorage();
	bool controlJSProgram(const QString &jsFileName,bool start);
	QStringList jsPrograms();
	IotProxyDevices* devices();
	DataCollectionUnit* collectionUnit(const QUuid &deviceId,const QString &sensorName);

private slots:
	void onStorageCreated(const DeviceStorageId &id);
	void onStorageRemoved(const DeviceStorageId &id);
	void onDeviceIdentified(QUuid id,QString name);
	void onDeviceDisconnected(QUuid id);

private:
	void setUserAndGroup();
	void loadDataProcessingScripts();
	void checkDataCollectionUnit(ARpcRealDevice *dev,const ARpcSensor &s);

public:
	bool terminated;

private:
	bool ready;
	ARpcConfig cfg;
	CmdArgParser cmdParser;
	QMap<QString,IExternCommandSource*> extCommands;
	IotProxyControlSocket localControl;
	IotProxyRemoteControlSocket remoteControl;
	QMap<QUuid,QMap<QString,DataCollectionUnit*>> collectionUnits;
	ARpcLocalDatabase *sensorsDb;
	IotProxyDevices *mDevices;
	QMap<QString,JSThread*> jsThreads;
};

#endif // IOTPROXYINSTANCE_H
