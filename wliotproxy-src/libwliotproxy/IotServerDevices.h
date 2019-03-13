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

#ifndef IOTSERVERDEVICES_H
#define IOTSERVERDEVICES_H

#include <QObject>
#include "IotServerCommands.h"
#include "IotServerConnection.h"
#include "IotServerDevice.h"
#include "IotServerVirtualDeviceClient.h"
#include "IotServerIDevicesSource.h"

class IotServerDevices
	:public IotServerIDevicesSource
{
	struct VDevCfg
	{
		QByteArray name;
		QList<SensorDef> sensors;
		ControlsGroup controls;
	};
	Q_OBJECT
public:
	explicit IotServerDevices(IotServerConnection *conn,IotServerCommands *cmds,QObject *parent=nullptr);
	virtual QList<IotServerTtyPortDescr> ttyPortsList()override;
	virtual bool identifyTty(const QByteArray &portName)override;
	virtual bool identifyTcp(const QByteArray &host)override;
	virtual QList<QUuid> identifiedDevices()override;
	virtual RealDevice* devById(const QUuid &id)override;
	virtual RealDevice* findDevByIdOrName(const QByteArray &idOrName)override;
	IotServerVirtualDeviceClient* registeredVDev(const QUuid &id);
	bool registerVirtualDevice(const QUuid &deviceId,const QByteArray &deviceName,
		const QList<SensorDef> &sensors,const ControlsGroup &controls);

private slots:
	void onServerConnected();
	void onServerDisconnected();
	void onDeviceIdentifiedFromServer(const QUuid &id,const QByteArray &name,const QUuid &typeId);
	void onDeviceLostFromServer(const QUuid &id);
	void onDeviceStateChanged(const QUuid &id,const QByteArrayList &args);
	void onDeviceConnected();
	void onDeviceDisconnected();
	void onVDevMsg(const QUuid &id,const Message &m);

private:
	IotServerCommands *commands;
	IotServerConnection *srvConn;
	QMap<QUuid,IotServerDevice*> devices;
	QMap<QUuid,IotServerVirtualDeviceClient*> virtualDevices;
	QMap<QUuid,VDevCfg> registeredVDevIds;
};

#endif // IOTSERVERDEVICES_H
