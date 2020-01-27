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

#ifndef IOTSERVERDEVICEBACKEND_H
#define IOTSERVERDEVICEBACKEND_H

#include "wliot/devices/IHighLevelDeviceBackend.h"
#include "IotServerConnection.h"
#include "IotServerCommands.h"

class IotServerDeviceBackend
	:public IHighLevelDeviceBackend
{
	Q_OBJECT
public:
	explicit IotServerDeviceBackend(IotServerConnection *conn,IotServerCommands *cmds,const QUuid &id,
		const QByteArray &name,const QUuid &classId,const QByteArray &devType,const QByteArray &devPortName,
		QObject *parent=nullptr);
	virtual ~IotServerDeviceBackend();
	virtual bool writeMessageToDevice(const Message &m)override;
	virtual bool isConnected()const override;
	virtual void forceDisconnect()override;
	virtual QByteArray type()const override;
	virtual QByteArray portOrAddress()const override;
	virtual bool event(QEvent *event)override;

protected:
	virtual void writeMessageToDeviceFromQueue(const Message &m);

private:
	void stateChangedFromServer(const QByteArrayList &args);
	void setDisconnected();

protected:
	IotServerConnection *srvConn;
	IotServerCommands *commands;

private:
	friend class IotServerDevices;
	bool mConnected;
	QUuid devId;
	QByteArray devName;
	QUuid devClassId;
	QByteArray mType,mPortName;
};

#endif // IOTSERVERDEVICEBACKEND_H
