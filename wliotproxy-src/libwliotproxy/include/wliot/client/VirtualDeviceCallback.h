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

#ifndef VIRTUALDEVICECALLBACK_H
#define VIRTUALDEVICECALLBACK_H

#include <QObject>
#include "wliot/devices/DeviceState.h"

namespace WLIOTClient
{
	class VirtualDeviceClient;

	class VirtualDeviceCallback
		:public QObject
	{
		Q_OBJECT
	public:
		explicit VirtualDeviceCallback();

	protected:
		virtual bool processCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)=0;
		virtual void prepareState(WLIOT::DeviceState &);
		void commandParamStateChanged(const QByteArray &cmd,quint32 paramIndex,const QByteArray &value);
		void additionalStateChanged(const QByteArray &paramName,const QByteArray &value);

	protected:
		friend class DevicesList;
		friend class VirtualDeviceClient;
		VirtualDeviceClient *devClient;
	};
}

#endif // VIRTUALDEVICECALLBACK_H
