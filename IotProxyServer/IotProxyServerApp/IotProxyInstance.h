#ifndef IOTPROXYINSTANCE_H
#define IOTPROXYINSTANCE_H

#include "ARpcDevices/ARpcTtyDevice.h"
#include "ARpcDevices/ARpcTcpDevice.h"
#include "CmdArgParser.h"
#include "IotProxyControlSocket.h"
#include "ARpcLocalStorage/ARpcLocalDatabase.h"
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

private slots:
	void devMsgHandler(const ARpcMessage &m);
	void onTtyDeviceIdentified();
	void onTcpDeviceIdentified();
	void onTtyDeviceDisconnected();
	void onTcpDeviceDisconnected();

private:
	void setupControllers();
	void setUserAndGroup();
	QStringList extractTtyPorts();

public:
	bool terminateProxy;

private:
	bool ready;
	ARpcConfig cfg;
	CmdArgParser cmdParser;
	QMap<QUuid,ARpcTtyDevice*> identifiedTtyDevices;
	QList<ARpcTtyDevice*> allTtyDevices;
	QMap<QUuid,ARpcTcpDevice*> identifiedTcpDevices;
	QList<ARpcTcpDevice*> allTcpDevices;
	QString cfgDir;
	QString daemonVarDir;
	IotProxyControlSocket ctlSocket;
	ARpcLocalDatabase *sensorsDb;
};

#endif // IOTPROXYINSTANCE_H
