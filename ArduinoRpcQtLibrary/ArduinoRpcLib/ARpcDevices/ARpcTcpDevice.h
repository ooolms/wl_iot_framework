#ifndef ARPCTCPDEVICE_H
#define ARPCTCPDEVICE_H

#include "ARpcBase/ARpcStreamParser.h"
#include "ARpcBase/ARpcMessageParser.h"
#include "ARpcBase/ARpcDevice.h"
#include <QObject>
#include <QHostAddress>
#include <QSslSocket>
#include <QTimer>

//tcp port 4081
//TODO ssl support
class ARpcTcpDevice
	:public ARpcDevice
{
	Q_OBJECT
public:
	explicit ARpcTcpDevice(const QHostAddress &addr,QObject *parent=0);
	virtual bool writeMsg(const ARpcMessage &m)override;
	virtual bool isConnected()override;
	QHostAddress getAddress()const;

private slots:
	void onRetryTimer();
	void onSocketConnected();
	void onSocketDisonnected();

public:
	static const quint16 stdDevPort;

private:
	QHostAddress address;
	QSslSocket socket;
	quint16 port;
	QTimer retryTimer;
};

#endif // ARPCTCPDEVICE_H
