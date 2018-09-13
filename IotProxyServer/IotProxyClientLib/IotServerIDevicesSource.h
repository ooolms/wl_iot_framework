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

#ifndef IOTSERVERIDEVICESSOURCE_H
#define IOTSERVERIDEVICESSOURCE_H

#include <QObject>
#include "ARpcBase/ARpcRealDevice.h"
#include "IotServerTypes.h"

class IotServerIDevicesSource
	:public QObject
{
	Q_OBJECT
public:
	explicit IotServerIDevicesSource(QObject *parent = nullptr);
	virtual QList<IotServerTtyPortDescr> ttyPortsList()=0;
	virtual bool identifyTty(const QByteArray &portName)=0;
	virtual bool identifyTcp(const QByteArray &host)=0;
	virtual QList<QUuid> identifiedDevices()=0;
	virtual ARpcRealDevice *devById(const QUuid &id)=0;

signals:
	void deviceIdentified(const QUuid &id,const QByteArray &name);
	void deviceLost(const QUuid &id);
};

#endif // IOTSERVERIDEVICESSOURCE_H
