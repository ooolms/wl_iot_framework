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

#ifndef VIRTUALDEVICEBACKEND_H
#define VIRTUALDEVICEBACKEND_H

#include "wliot/devices/IHighLevelDeviceBackend.h"
#include <QUuid>

//TODO переделать void* clientPtr на что-нибудь более приличное (callbacks?)
namespace WLIOT
{
	class VirtualDeviceBackend;

	class IVirtualDeviceBackendCallback
	{
	public:
		virtual ~IVirtualDeviceBackendCallback(){}
		virtual void onMessageToVDev(VirtualDeviceBackend *vDev,const Message &m)=0;
	};

	/**
	 * @brief Специальный backend для виртуальных устройств
	 */
	class VirtualDeviceBackend
		:public IHighLevelDeviceBackend
	{
		Q_OBJECT
	public:
		explicit VirtualDeviceBackend(
			const QUuid &id,const QByteArray &name,const QUuid &typeId,
			IVirtualDeviceBackendCallback *cb,QObject *parent=nullptr);
		virtual ~VirtualDeviceBackend();
		void emulateMessageFromDevice(const Message &m);
		QUuid devId()const;
//		void setConnected(bool c);
		IVirtualDeviceBackendCallback* msgCb();
		virtual bool writeMessageToDevice(const Message &m)override;
		virtual bool isConnected()const override;
		virtual void forceDisconnect()override;
		virtual QByteArray backendType()const override;
		virtual QString hwAddress()const override;

	private:
		Q_INVOKABLE void writeMsgToDeviceQueued(WLIOT::Message m);

	public:
		static const QByteArray mBackendType;

	private:
		QUuid mDevId;
		QByteArray mDevName;
		QUuid mDevTypeId;
		IVirtualDeviceBackendCallback* mMsgCb;
	};
}

#endif // VIRTUALDEVICEBACKEND_H
