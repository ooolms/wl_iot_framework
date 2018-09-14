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
#include "IotServerIDevicesSource.h"

class IotServerDevices
	:public IotServerIDevicesSource
{
	Q_OBJECT
public:
	explicit IotServerDevices(IotServerConnection *conn,IotServerCommands *cmds,QObject *parent=nullptr);
	virtual QList<IotServerTtyPortDescr> ttyPortsList()override;
	virtual bool identifyTty(const QByteArray &portName)override;
	virtual bool identifyTcp(const QByteArray &host)override;
	virtual QList<QUuid> identifiedDevices()override;
	virtual ARpcRealDevice* devById(const QUuid &id)override;
	virtual ARpcRealDevice* findDevByIdOrName(const QByteArray &idOrName)override;

signals:
	void deviceIdentified(const QUuid &id,const QByteArray &name);
	void deviceLost(const QUuid &id);

private slots:
	void onServerConnected();
	void onDeviceIdentifiedFromServer(const QUuid &id,const QByteArray &name);
	void onDeviceLostFromServer(const QUuid &id);
	void onDeviceConnected();
	void onDeviceDisconnected();

private:
	IotServerCommands *commands;
	IotServerConnection *srvConn;
	QMap<QUuid,IotServerDevice*> devices;
};

#endif // IOTSERVERDEVICES_H
