#ifndef IOTSERVERVIRTUALDEVICE_H
#define IOTSERVERVIRTUALDEVICE_H

#include "IotServerDevice.h"

//CRIT check and rewrite work logic
class IotServerVirtualDevice
	:public IotServerDevice
{
	Q_OBJECT
public:
	explicit IotServerVirtualDevice(IotServerConnection *conn,IotServerCommands *cmds,const QUuid &id,
		const QByteArray &name,const QByteArray &type,QObject *parent=nullptr);
	void sendVDevMeasurement(const QByteArray &sensor,const QByteArrayList &args);
	void sendVDevMeasurementB(const QByteArray &sensor,const SensorValue &val);

signals:
	void processVirtualDeviceCommand(const QByteArray &cmd,const QByteArrayList &args,bool &ok,QByteArrayList &retVal);

protected:
	virtual void processMessage(const Message &m) override;

private:
	friend class IotServerDevices;
};

#endif // IOTSERVERVIRTUALDEVICE_H
