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

class ARpcHubDevice;

/**
 * @brief The ARpcRealDevice class
 * Абстрактное устройство с идентификатором, именем, сенсорами и интерфейсом управления.
 * Данный класс реализует процесс идентификации, запрос списка датчиков и интерфейса управления.
 */
class ARpcRealDevice
	:public ARpcDevice
{
	Q_OBJECT
public:
	enum IdentifyResult
	{
		FAILED,
		OK,
		OK_NULL_ID_OR_NAME
	};

public:
	explicit ARpcRealDevice(QObject *parent=0);
	virtual ~ARpcRealDevice();
	bool isIdentified();
	QUuid id();
	QByteArray name();//human-readable
	bool getSensorsDescription(QList<ARpcSensorDef> &sensors);
	bool getControlsDescription(ARpcControlsGroup &controls);
	bool getState(ARpcDeviceState &state);
	bool isHubDevice();
	QList<QUuid> childDevices();
	ARpcRealDevice* childDevice(const QUuid &id);
	bool identifyHub();
	bool controlsLoaded();
	bool sensorsLoaded();
	virtual QByteArray deviceType()=0;

public slots:
	IdentifyResult identify();

signals:
	void identificationChanged(const QUuid &oldId,const QUuid &newId);
	void childDeviceIdentified(const QUuid &deviceId);
	void childDeviceLost(const QUuid &deviceId);
	void stateChanged(const QByteArrayList &args);

protected:
	void resetIdentification(QUuid newId=QUuid(),QByteArray newName=QByteArray());
	virtual void syncFailed();

private slots:
	void onDisconnected();
	void onNewMessage(const ARpcMessage &m);
	void onSyncTimer();
	void onChildDeviceSyncFailed();

private:
	void onHubMsg(const ARpcMessage &m);
	void onHubDeviceIdentified(const QUuid &id,const QByteArray &name);

protected://для потомков
	QUuid devId;
	QByteArray devName;
	bool hubDevice;
	QTimer syncTimer;
	bool mWasSyncr;

private:
	QTimer identifyTimer;
	QMap<QUuid,ARpcHubDevice*> hubDevicesMap;
	QList<ARpcSensorDef> mSensors;
	ARpcControlsGroup mControls;
	ARpcDeviceState mState;
	bool mControlsLoaded;
	bool mSensorsLoaded;
	bool mStateLoaded;
};

#endif // ARPCREALDEVICE_H

