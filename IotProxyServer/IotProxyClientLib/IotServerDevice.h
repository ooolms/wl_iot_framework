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

#include "ARpcBase/ARpcRealDevice.h"
#include "IotServerConnection.h"
#include "IotServerCommands.h"

class IotServerDevice
	:public ARpcRealDevice
{
	Q_OBJECT
public:
	explicit IotServerDevice(IotServerConnection *conn,IotServerCommands *cmds,const QUuid &id,
		const QByteArray &name,QObject *parent=nullptr);
	void setDeviceConnected(bool c);

public:
	virtual bool writeMsg(const ARpcMessage &m)override;
	virtual bool isConnected()override;

private slots:
	void processMessages();
	void onServerConnectionChanged();

private:
	void processMessage(const ARpcMessage &m);

private:
	IotServerConnection *srvConn;
	IotServerCommands *commands;
	bool deviceConnectedFlag;
	QUuid devId;
	QByteArray devName;
	QList<ARpcMessage> messagesToDevice;
};

#endif // IOTSERVERDEVICE_H
