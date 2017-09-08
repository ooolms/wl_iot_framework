#ifndef QSSLSERVER_H
#define QSSLSERVER_H

#include <QTcpServer>

class QSslServer
	:public QTcpServer
{
	Q_OBJECT
public:
	explicit QSslServer(QObject *parent=0);

protected:
	virtual void incomingConnection(qintptr socketDescriptor);
};

#endif // QSSLSERVER_H
