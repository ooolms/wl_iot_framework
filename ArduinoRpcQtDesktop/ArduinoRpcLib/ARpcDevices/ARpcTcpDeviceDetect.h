#ifndef ARPCTCPDEVICEDETECT_H
#define ARPCTCPDEVICEDETECT_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include "ARpcDevices/ARpcTcpDeviceDetectServer.h"

class ARpcTcpDeviceDetect
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcTcpDeviceDetect(QObject *parent=0);
	bool isServerListening();
	void startRegularBroadcasting(int msecsDelay);
	void stopRegularBroadcasting();

public slots:
	void broadcastServerReadyMessage();

signals:
	void newClient(qintptr s,bool &accepted);

private:
	ARpcTcpDeviceDetectServer srv;
	QUdpSocket bCastSock;
	QTimer tmr;
};

#endif // ARPCTCPDEVICEDETECT_H
