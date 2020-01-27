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

#ifndef REALDEVICE_H
#define REALDEVICE_H

#include "wliot/devices/Message.h"
#include "wliot/devices/SensorDef.h"
#include "wliot/devices/ControlsDefinition.h"
#include "wliot/devices/DeviceState.h"
#include "wliot/devices/CommandCall.h"
#include "wliot/devices/IHighLevelDeviceBackend.h"
#include <QTimer>
#include <QUuid>
#include <QSharedPointer>

class HubDevice;

/**
 * @brief The RealDevice class
 * Устройство с идентификатором, именем, сенсорами и интерфейсом управления.
 * Данный класс реализует процесс идентификации, запрос списка датчиков и интерфейса управления.
 */
class RealDevice
	:public QObject
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
	explicit RealDevice(QObject *parent=nullptr);
	virtual ~RealDevice();
	void setBackend(IHighLevelDeviceBackend *b);
	IHighLevelDeviceBackend* takeBackend();
	IHighLevelDeviceBackend* backend();
	IdentifyResult identify();
	bool isReady();
	QUuid id();
	QUuid classId();
	QByteArray name();//human-readable
	void renameDevice(const QByteArray &newName,bool silent=true);
		//silent - name updated without emiting nameChanged signal
	bool getSensorsDescription(QList<SensorDef> &sensors);
	bool getControlsDescription(ControlsGroup &controls);
	bool getState(DeviceState &state);
	bool writeMsgToDevice(const Message &m);
	bool isConnected()const;
	QSharedPointer<CommandCall> execCommand(CommandCall *call);
	QSharedPointer<CommandCall> execCommand(const QByteArray &cmd);
	QSharedPointer<CommandCall> execCommand(const QByteArray &cmd,const QByteArrayList &args);

public://for hub devices
	bool isHubDevice();
	QList<QUuid> childDevices();
	HubDevice* childDevice(const QUuid &id);
	bool identifyHub();

signals:
	void disconnected();
	void connected();
	void identified();
	void newMessageFromDevice(const Message &m);
	void deviceWasReset();
	void stateChanged(const QByteArrayList &args);
	void nameChanged(const QByteArray &newName);

	void childDeviceIdentified(const QUuid &deviceId);
	void childDeviceLost(const QUuid &deviceId);

protected://internal API
	virtual void syncFailed();

protected slots://internal API
	void onConnected();
	void onDisconnected();
	void onDeviceReset();
	void onNewMessage(const Message &m);

private slots:
	void onSyncTimer();
	void onChildDeviceSyncFailed();
	void onCommandDone();

private:
	void onHubMsg(const Message &m);
	void onHubDeviceIdentified(const QUuid &id,const QByteArray &name);
	void stopBackend();

protected://для потомков
	QUuid devId;
	QByteArray devName;
	QUuid devTypeId;
	bool hubDevice;
	QTimer syncTimer;
	int mSyncCounter;

private:
	IHighLevelDeviceBackend *mBackend;
	QMap<QUuid,HubDevice*> hubDevicesMap;
	QMap<QByteArray,QSharedPointer<CommandCall>> execCommands;
	CommandCall *identifyCall;
	QList<SensorDef> mSensors;
	ControlsGroup mControls;
	DeviceState mState;
	bool mControlsLoaded;
	bool mSensorsLoaded;
	bool mStateLoaded;
	bool mConnected;
	qint64 callId;
};

#endif // REALDEVICE_H
