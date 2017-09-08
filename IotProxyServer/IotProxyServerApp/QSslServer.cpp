#include "QSslServer.h"
#include <QSslSocket>

QSslServer::QSslServer(QObject *parent)
	:QTcpServer(parent)
{
}

void QSslServer::incomingConnection(qintptr socketDescriptor)
{
	QSslSocket *serverSocket=new QSslSocket;
	if(serverSocket->setSocketDescriptor(socketDescriptor))
	{
		addPendingConnection(serverSocket);
		serverSocket->startServerEncryption();
	}
	else delete serverSocket;
}

