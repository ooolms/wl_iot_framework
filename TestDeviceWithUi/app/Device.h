#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QUdpSocket>
#include <QTcpServer>
#include <QTcpSocket>
#include "ARpcDevice.h"
#include "ARpcSrvReady.h"

class Device
	:public QObject
	,public ARpcISrvReadyCallback
	,public ARpcIWriteCallback
{
	Q_OBJECT
public:
	explicit Device(QObject *parent=nullptr);
	bool connectToServer(const QHostAddress &addr);
	void processSrvReadyMsg(const ARpcUuid &serverId,const char *serverName);
	void writeData(const char *data,unsigned long sz);
	void writeStr(const char *str);

private slots:
	void onBCastCliReadyRead();
	void onSocketReadyRead();

private:
	ARpcSrvReady srvReady;
	ARpcDevice dev;
	QUdpSocket *bCastCli;
	QHostAddress bCastSerderAddr;
	QTcpSocket *client;
};

#endif // DEVICE_H
