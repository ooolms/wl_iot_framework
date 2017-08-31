#ifndef ARPCTCPDEVICEDETECT_H
#define ARPCTCPDEVICEDETECT_H

#include <QObject>
#include <QTcpServer>
#include <QUdpSocket>
#include <QTimer>

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
	void newClient(QTcpSocket *sock,bool &accepted);

private slots:
	void onNewClient();

private:
	QTcpServer srv;
	QUdpSocket bCastSock;
	QTimer tmr;
};

#endif // ARPCTCPDEVICEDETECT_H
