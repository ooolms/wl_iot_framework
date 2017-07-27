#ifndef IOTPROXYINSTANCE_H
#define IOTPROXYINSTANCE_H

#include "ARpcDevices/ARpcTtyDevice.h"
#include "ARpcDevices/ARpcTcpDevice.h"
#include "CmdArgParser.h"
#include "IotProxyControlSocket.h"
#include "ARpcLocalStorage/ARpcLocalDatabase.h"
#include "DataCollectionUnit.h"
#include "LsTtyUsbDevices.h"
#include <QLocalServer>
#include <QLocalSocket>

class IotProxyInstance
	:public QObject
{
	Q_OBJECT
private:
	IotProxyInstance();
	IotProxyInstance(const IotProxyInstance &t);
	IotProxyInstance& operator=(const IotProxyInstance &t);
	~IotProxyInstance();

public:
	static IotProxyInstance& inst();
	void setup(int argc,char **argv);
	ARpcTtyDevice* findTtyDevByPortName(const QString &portName);
	ARpcTcpDevice* findTcpDevByAddress(const QHostAddress &address);
	ARpcDevice* deviceById(const QUuid &id);
	ARpcDevice* deviceByName(const QString &name);
	ARpcDevice* deviceByIdOrName(const QString str);
	ARpcLocalDatabase* getSensorsDb();
	bool findUsbTtyDeviceByPortName(const QString &portName,LsTtyUsbDevices::DeviceInfo &info);
	void terminate();

private slots:
	void devMsgHandler(const ARpcMessage &m);
	void onTtyDeviceIdentified();
	void onTcpDeviceIdentified();
	void onTtyDeviceDisconnected();
	void onTcpDeviceDisconnected();
	void onStorageCreated(const DeviceAndSensorId &id);
	void onStorageRemoved(const DeviceAndSensorId &id);

private:
	void setupControllers();
	void setUserAndGroup();
	QStringList extractTtyPorts();
	void deviceIdentified(ARpcDevice *dev);
	void checkDataCollectionUnit(ARpcDevice *dev,const ARpcSensor &s,const DeviceAndSensorId &stId);

private:
	bool ready;
	ARpcConfig cfg;
	CmdArgParser cmdParser;
	QList<ARpcTtyDevice*> allTtyDevices;
	QList<ARpcTcpDevice*> allTcpDevices;
	QMap<QUuid,ARpcDevice*> identifiedDevices;
	QMap<QUuid,QMap<QString,DataCollectionUnit*>> collectionUnits;
	QString cfgDir;
	QString daemonVarDir;
	IotProxyControlSocket ctlSocket;
	ARpcLocalDatabase *sensorsDb;
	QList<LsTtyUsbDevices::DeviceInfo> allTtyUsbDevices;
};

#endif // IOTPROXYINSTANCE_H
