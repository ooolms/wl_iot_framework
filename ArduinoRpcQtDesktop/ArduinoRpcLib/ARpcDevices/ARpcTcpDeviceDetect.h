#ifndef ARPCTCPDEVICEDETECT_H
#define ARPCTCPDEVICEDETECT_H

#include <QObject>
#include <QTcpServer>
#include <QUdpSocket>

class ARpcTcpDeviceDetect
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcTcpDeviceDetect(QObject *parent=0);
	void broadcastServerReadyMessage();

signals:
	void newClient(QTcpSocket *sock,bool &accepted);

private slots:
	void onNewClient();

private:
	QTcpServer srv;
	QUdpSocket bCastSock;
};

#endif // ARPCTCPDEVICEDETECT_H
