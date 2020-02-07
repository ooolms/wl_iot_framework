#ifndef IOTSERVERVIRTUALDEVICECLIENT_H
#define IOTSERVERVIRTUALDEVICECLIENT_H

#include "IotServerConnection.h"
#include "wliot/devices/DeviceState.h"
#include "wliot/devices/ControlsDefinition.h"
#include "wliot/devices/SensorDef.h"

class IotServerVirtualDeviceCallback;

//TODO hub support
//TODO убрать автоматическое создание состояний элем. управления из описания
class IotServerVirtualDeviceClient
	:public QObject
{
	Q_OBJECT
public:
	explicit IotServerVirtualDeviceClient(IotServerConnection *conn,const QUuid &id,const QByteArray &name,
		const QUuid &typeId,const QList<SensorDef> &sensors,const ControlsGroup &controls,QObject *parent=nullptr);
	void setDevEventsCallback(IotServerVirtualDeviceCallback *cb);
	void setupAdditionalStateAttributes(const QByteArrayList &names);
	void sendVDevMeasurement(const QByteArray &sensor,const QByteArrayList &args);
	void sendVDevMeasurementB(const QByteArray &sensor,const SensorValue &val);
	void writeInfo(const QByteArrayList &args);

private:
	void onNewMessageFromServer(const Message &m);
	void writeOk(const QByteArray &callId,const QByteArrayList &args);
	void writeErr(const QByteArray &callId,const QByteArrayList &args);
	void prepareStateFromControls(const ControlsGroup &grp);
	void commandParamStateChanged(const QByteArray &cmd,quint32 paramIndex,const QByteArray &value);
	void additionalStateChanged(const QByteArray &paramName,const QByteArray &value);

private:
	friend class IotServerDevices;
	friend class IotServerVirtualDeviceCallback;
	IotServerConnection *srvConn;
	IotServerVirtualDeviceCallback *callback;
	QUuid devId;
	QUuid devTypeId;
	QByteArray devName;
	QList<SensorDef> mSensors;
	ControlsGroup mControls;
	DeviceState mState;
};

#endif // IOTSERVERVIRTUALDEVICECLIENT_H
