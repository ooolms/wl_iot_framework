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

#ifndef ARPCREALDEVICE_H
#define ARPCREALDEVICE_H

#include "ARpcDevice.h"
#include <QTimer>

class ARpcRealDevice
	:public ARpcDevice
{
	Q_OBJECT
public:
	explicit ARpcRealDevice(QObject *parent=0);
	virtual ~ARpcRealDevice(){}
	bool isIdentified();
	QUuid id();
	QString name();//human-readable
	bool getSensorsDescription(QList<ARpcSensor> &sensors);
	bool getControlsDescription(ARpcControlsGroup &controls);
	bool getState(ARpcDeviceState &state);
	bool isHubDevice();

public slots:
	bool identify();

signals:
	void identificationChanged();

protected:
	void resetIdentification();

private slots:
	void onDisconnected();

protected://для потомков
	QUuid devId;
	QString devName;
	bool hubDevice;

private:
	QTimer identifyTimer;
};

#endif // ARPCREALDEVICE_H
