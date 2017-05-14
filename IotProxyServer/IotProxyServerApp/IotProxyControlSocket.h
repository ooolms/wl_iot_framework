#ifndef IOTPROXYCONTROLSOCKET_H
#define IOTPROXYCONTROLSOCKET_H

#include <QObject>
#include <QLocalServer>
#include <QLocalSocket>
#include "ARpcBase/ARpcOutsideDevice.h"
#include "IotProxyCommandProcessor.h"

class IotProxyControlSocket
	:public QObject
{
	Q_OBJECT
public:
	explicit IotProxyControlSocket(QObject *parent=0);
	virtual ~IotProxyControlSocket();

private slots:
	void onNewLocalConnection();
	void onLocalSocketDisconnected();

private:
	QLocalServer localServer;
	QList<QLocalSocket*> localClients;
	QList<ARpcOutsideDevice*> clientDevices;
	QList<IotProxyCommandProcessor*> clientCmdProcs;
};

#endif // IOTPROXYCONTROLSOCKET_H
