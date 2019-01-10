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

#ifndef IOTSERVERDEVICESCOMMANDS_H
#define IOTSERVERDEVICESCOMMANDS_H

#include <QObject>
#include <QUuid>
#include "ARpcBase/ARpcSensorDef.h"
#include "ARpcBase/ARpcControlsDefinition.h"
#include "IotServerTypes.h"
#include "IotServerConnection.h"

class IotServerDevicesCommands
	:public QObject
{
	Q_OBJECT
public:
	explicit IotServerDevicesCommands(IotServerConnection *conn,QObject *parent=nullptr);
	bool listTty(QList<IotServerTtyPortDescr> &ttyList);
	bool identifyTty(const QByteArray &portName);
	bool identifyTcp(const QByteArray &host);
	bool listIdentified(QList<IotServerIdentifiedDeviceDescr> &devs);
	bool listSensors(const QByteArray &idOrName,QList<ARpcSensorDef> &sensors);
	bool listControls(const QByteArray &idOrName,ARpcControlsGroup &controls);
	bool deviceId(const QByteArray &idOrName,QUuid &id);
	bool execDeviceCommand(const QByteArray &idOrName,const QByteArray &command,
		const QByteArrayList &args,QByteArrayList &retVal);
	bool registerVirtualDevice(const QUuid &deviceId,const QByteArray &deviceName,
		const QByteArray &sensorsStr,const QByteArray &controlsStr);
	bool sendVDevMeasurement(const QUuid &deviceId,const QByteArray &sensorName,const QByteArrayList &measArgs);

private:
	IotServerConnection *srvConn;
};

#endif // IOTSERVERDEVICESCOMMANDS_H
