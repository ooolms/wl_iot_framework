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

#ifndef DEVICESLIST_H
#define DEVICESLIST_H

#include <QObject>
#include "wliot/client/AllServerCommands.h"
#include "wliot/client/ServerConnection.h"
#include "wliot/client/VirtualDeviceClient.h"
#include "wliot/client/IDevicesSource.h"

namespace WLIOTClient
{
	class DevicesList
		:public IDevicesSource
	{
		struct VDevCfg
		{
			QUuid typeId;
			QByteArray name;
			QList<WLIOT::SensorDef> sensors;
			WLIOT::ControlsGroup controls;
			VirtualDeviceCallback *cb;
		};
		Q_OBJECT
	public:
		explicit DevicesList(ServerConnection *conn,AllServerCommands *cmds);
		virtual QList<TtyPortDescr> ttyPortsList()override;
		virtual bool identifyTty(const QByteArray &portName)override;
		virtual bool identifyTcp(const QByteArray &host)override;
		virtual QList<QUuid> identifiedDevices()override;
		virtual WLIOT::RealDevice* devById(const QUuid &id)override;
		virtual WLIOT::RealDevice* findDevByIdOrName(const QByteArray &idOrName)override;
		VirtualDeviceClient* registeredVirtualDevice(const QUuid &id);
		VirtualDeviceClient* registerVirtualDevice(const QUuid &deviceId,const QByteArray &deviceName,
			const QList<WLIOT::SensorDef> &sensors,const WLIOT::ControlsGroup &controls,
			const QUuid &typeId,VirtualDeviceCallback *cb);
		void disconnectVirtualDevice(const QUuid &devId);

	private slots:
		void onServerConnected();
		void onServerDisconnected();
		void onDeviceIdentifiedFromServer(const QUuid &id,const QByteArray &name,const QUuid &typeId);
		void onDeviceLostFromServer(const QUuid &id);
		void onDeviceStateChanged(const QUuid &id,const QByteArrayList &args);
		void onVDevMsg(const QUuid &id,const WLIOT::Message &m);

	private:
		VirtualDeviceClient* setupVDev(const QUuid &devId,const VDevCfg &cfg);

	private:
		AllServerCommands *commands;
		ServerConnection *srvConn;
		QMap<QUuid,WLIOT::RealDevice*> devices;
		QMap<QUuid,VirtualDeviceClient*> virtualDevices;
		QMap<QUuid,VDevCfg> registeredVDevIds;
	};
}

#endif // DEVICESLIST_H
