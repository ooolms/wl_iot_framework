#ifndef IOTPROXYINSTANCE_H
#define IOTPROXYINSTANCE_H

#include "ARpcDevices/ARpcTtyDevice.h"
#include "ARpcDevices/ARpcTcpDevice.h"
#include "CmdArgParser.h"
#include "IotProxyControlSocket.h"
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

private slots:
	void devMsgHandler(const ARpcMessage &m);

private:
	void setupControllers();
	void setUserAndGroup();

public:
	bool terminateProxy;

private:
	bool ready;
	ARpcConfig cfg;
	CmdArgParser cmdParser;
	QMap<QUuid,ARpcTtyDevice*> ttyDevices;
	QMap<QUuid,ARpcTcpDevice*> tcpDevices;
	QString cfgDir;
	IotProxyControlSocket ctlSocket;
};

#endif // IOTPROXYINSTANCE_H
