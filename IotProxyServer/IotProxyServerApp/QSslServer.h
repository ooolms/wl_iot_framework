#ifndef QSSLSERVER_H
#define QSSLSERVER_H

#include <QTcpServer>
#include <QSslCertificate>
#include <QSslKey>

class QSslServer
	:public QTcpServer
{
	Q_OBJECT
public:
	explicit QSslServer(QObject *parent=0);
	void setSslOptions(const QSslCertificate &c,const QSslKey &k);

protected:
	virtual void incomingConnection(qintptr socketDescriptor);

private:
	QSslCertificate crt;
	QSslKey key;
};

#endif // QSSLSERVER_H
