#ifndef IOTSERVERDEVICESCOMMANDS_H
#define IOTSERVERDEVICESCOMMANDS_H

#include <QObject>
#include <QUuid>
#include "ARpcBase/ARpcSensorDef.h"
#include "ARpcBase/ARpcControlsDefinition.h"
#include "IotServerTypes.h"
#include "IotServerConnection.h"

class IotServerDevicesCommands
	:public QObject
{
	Q_OBJECT
public:
	explicit IotServerDevicesCommands(IotServerConnection *conn,QObject *parent=nullptr);
	bool listTty(QList<IotServerTtyPortDescr> &ttyList);
	bool identifyTty(const QByteArray &portName);
	bool identifyTcp(const QByteArray &host);
	bool listIdentified(QList<IotServerIdentifiedDeviceDescr> &devs);
	bool listSensors(const QByteArray &idOrName,QList<ARpcSensorDef> &sensors);
	bool listControls(const QByteArray &idOrName,ARpcControlsGroup &controls);
	bool deviceId(const QByteArray &idOrName,QUuid &id);
	bool execDeviceCommand(const QByteArray &idOrName,const QByteArray &command,
		const QByteArrayList &args,QByteArrayList &retVal);

private:
	IotServerConnection *srvConn;
};

#endif // IOTSERVERDEVICESCOMMANDS_H
