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

#ifndef IOTSERVERDEVICE_H
#define IOTSERVERDEVICE_H

#include "wliot/devices/RealDevice.h"
#include "IotServerConnection.h"
#include "IotServerCommands.h"

//CRIT check work logic!!!!
class IotServerDevice
	:public RealDevice
{
	Q_OBJECT
public:
	explicit IotServerDevice(IotServerConnection *conn,IotServerCommands *cmds,const QUuid &id,
		const QByteArray &name,const QByteArray &type,QObject *parent=nullptr);
	void setDeviceConnected(bool c);
	virtual QByteArray deviceType(){return devType;}
	void stateChangedFromServer(const QByteArrayList &args);

public:
	virtual bool writeMsgToDevice(const Message &m)override;

protected:
	virtual void processMessage(const Message &m);

private slots:
	void processMessages();
	void onServerConnectionChanged();

protected:
	IotServerConnection *srvConn;
	IotServerCommands *commands;

private:
	bool deviceConnectedFlag;
	QUuid devId;
	QByteArray devName;
	QByteArray devType;
	QList<Message> messagesToDevice;
};

#endif // IOTSERVERDEVICE_H
