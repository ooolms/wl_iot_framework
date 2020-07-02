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

#ifndef SERVERDEVICEBACKEND_H
#define SERVERDEVICEBACKEND_H

#include "wliot/devices/IHighLevelDeviceBackend.h"
#include "wliot/client/ServerConnection.h"
#include "wliot/client/AllServerCommands.h"

namespace WLIOTClient
{
	class ServerDeviceBackend
		:public WLIOT::IHighLevelDeviceBackend
	{
		Q_OBJECT
	public:
		explicit ServerDeviceBackend(ServerConnection *conn,AllServerCommands *cmds,const QUuid &id,
			const QByteArray &name,const QUuid &classId,const QByteArray &devType,const QByteArray &hwAddr,
			QObject *parent=nullptr);
		virtual ~ServerDeviceBackend();
		virtual bool writeMessageToDevice(const WLIOT::Message &m)override;
		virtual bool isConnected()const override;
		virtual void forceDisconnect()override;
		virtual QByteArray backendType()const override;
		virtual QString hwAddress()const override;
		virtual bool event(QEvent *event)override;

	protected:
		virtual void writeMessageToDeviceFromQueue(const WLIOT::Message &m);

	private:
		void stateChangedFromServer(const QByteArrayList &args);
		void setDisconnected();

	protected:
		ServerConnection *srvConn;
		AllServerCommands *commands;

	private:
		friend class DevicesList;
		bool mConnected;
		QUuid devId;
		QByteArray devName;
		QUuid devClassId;
		QByteArray mType,mHwAddress;
	};
}

#endif // SERVERDEVICEBACKEND_H
