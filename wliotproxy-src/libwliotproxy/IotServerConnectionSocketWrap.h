#ifndef IOTSERVERCONNECTIONSOCKETWRAP_H
#define IOTSERVERCONNECTIONSOCKETWRAP_H

#include <QObject>

class QLocalSocket;
class QSslSocket;
class IotServerConnection;



class IotServerConnectionSocketWrap
	:public QObject
{
	Q_OBJECT
public:
	explicit IotServerConnectionSocketWrap(IotServerConnection *conn);
	Q_INVOKABLE void startConnectLocal();
	Q_INVOKABLE void startConnectNet();
	Q_INVOKABLE void disconnectFromServer();
	Q_INVOKABLE void writeData(QByteArray data);

signals:
	void newData(QByteArray data);
	void connectionError();

private slots:
	void onLocalReadyRead();
	void onNetReadyRead();
	void onLocalSocketError();
	void onNetError();
	void onSslError();

public:
	union
	{
		QLocalSocket *localSock;
		QSslSocket *netSock;
	};
	IotServerConnection *connection;
};

#endif // IOTSERVERCONNECTIONSOCKETWRAP_H
