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

#ifndef IDEVICESSOURCE_H
#define IDEVICESSOURCE_H

#include <QObject>
#include "wliot/devices/RealDevice.h"
#include "wliot/client/ServerDataTypes.h"

namespace WLIOTClient
{
	class IDevicesSource
		:public QObject
	{
		Q_OBJECT
	public:
		explicit IDevicesSource(QObject *parent = nullptr);
		virtual QList<TtyPortDescr> ttyPortsList()=0;
		virtual bool identifyTty(const QByteArray &portName)=0;
		virtual bool identifyTcp(const QByteArray &host)=0;
		virtual QList<QUuid> identifiedDevices()=0;
		virtual WLIOT::RealDevice *devById(const QUuid &id)=0;
		virtual WLIOT::RealDevice *findDevByIdOrName(const QByteArray &idOrName)=0;

	signals:
		void deviceConnected(const QUuid &id);
		void deviceDisconnected(const QUuid &id);
	};
}

#endif // IDEVICESSOURCE_H
