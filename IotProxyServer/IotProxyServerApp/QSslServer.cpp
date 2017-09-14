#include "QSslServer.h"
#include <QSslSocket>

QSslServer::QSslServer(QObject *parent)
	:QTcpServer(parent)
{
}

void QSslServer::setSslOptions(const QSslCertificate &c,const QSslKey &k)
{
	crt=c;
	key=k;
}

void QSslServer::incomingConnection(qintptr socketDescriptor)
{
	QSslSocket *serverSocket=new QSslSocket(this);
	if(serverSocket->setSocketDescriptor(socketDescriptor))
	{
		addPendingConnection(serverSocket);
		serverSocket->setPeerVerifyMode(QSslSocket::VerifyNone);
		serverSocket->setPrivateKey(key);
		serverSocket->setLocalCertificate(crt);
		serverSocket->ignoreSslErrors(QList<QSslError>()<<QSslError::SelfSignedCertificate);
		serverSocket->startServerEncryption();
	}
	else
		delete serverSocket;
}
