/*******************************************
   Copyright 2017 OOO "LMS"

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.*/

#include "TcpControlSocket.h"
#include "wliot/WLIOTProtocolDefs.h"
#include "wliot/WLIOTServerProtocolDefs.h"
#include <QThread>
#include <QDebug>
#include <sys/stat.h>

using namespace WLIOT;

TcpControlSocket::TcpControlSocket(QObject *parent)
	:QObject(parent)
{
	connect(&sslServer,&SslServer::newConnection,this,&TcpControlSocket::onNewLocalConnection);
}

TcpControlSocket::~TcpControlSocket()
{
	for(auto &set:QList<ClientSet>(clients))
	{
		set.sock->disconnect(this);
		if(set.proc)
			set.proc->scheduleDelete();
		else set.sock->deleteLater();

		//		set->sock()->disconnect(this);
		//		set->quit();
		//		set->wait(2000);
		//		set->terminate();
		//		//TODO gracefull stop
		//		delete set;
	}
	clients.clear();
}

void TcpControlSocket::start(const QList<QSslCertificate> &crt,const QSslKey &key)
{
	if(sslServer.isListening())return;
	sslServer.setSslOptions(crt,key);
	sslServer.listen(QHostAddress::AnyIPv4,WLIOTServerProtocolDefs::controlSslPort);
}

void TcpControlSocket::stop()
{
	sslServer.close();
}

void TcpControlSocket::onNewLocalConnection()
{
	QSslSocket *sock=(QSslSocket*)sslServer.nextPendingConnection();
	while(sock!=0)
	{
		//TODO multithread ??? "QSocketNotifier: Socket notifiers cannot be enabled or disabled from another thread"
		qInfo()<<"Client connected";
		connect(sock,&QSslSocket::disconnected,this,&TcpControlSocket::onSocketDisconnected,
			Qt::QueuedConnection);
		connect(sock,&QSslSocket::encrypted,this,&TcpControlSocket::onSocketEncrypted,Qt::QueuedConnection);
		connect(sock,static_cast<void (QSslSocket::*)(
			const QList<QSslError>&)>(&QSslSocket::sslErrors),this,
			&TcpControlSocket::onSslErrors,Qt::QueuedConnection);
		connect(sock,static_cast<void (QSslSocket::*)(
			QAbstractSocket::SocketError)>(&QSslSocket::error),this,
			&TcpControlSocket::onSocketError,Qt::QueuedConnection);
		ClientSet set;
		set.sock=sock;
		clients.append(set);

		//		ClientThread *thr=new ClientThread(sock,this);
		//		thr->setup();
		//		clients.append(thr);

		sock=(QSslSocket*)sslServer.nextPendingConnection();
	}
}

void TcpControlSocket::onSocketDisconnected()
{
	closeClient((QSslSocket*)sender());
	//	ClientThread *thr=clients[index];
	//	thr->quit();
	//	thr->wait(2000);
	//	thr->terminate();
	//	delete thr;
	//	clients.removeAt(index);ARpcConfig
	//	qDebug()<<"Client disconnected";
}

void TcpControlSocket::onSocketEncrypted()
{
	QSslSocket *sock=(QSslSocket*)sender();
	int index=findClient(sock);
	if(index==-1)
		return;
	ClientSet &set=clients[index];
	CommandProcessor *cProc=new CommandProcessor(sock);
	set.proc=cProc;
}

void TcpControlSocket::onSocketError(QAbstractSocket::SocketError)
{
	QSslSocket *s=(QSslSocket*)sender();
	if(s->error()!=QSslSocket::RemoteHostClosedError)
		qWarning()<<"tcp control socket error: "<<s->errorString();
	s->disconnectFromHost();
}

void TcpControlSocket::onSslErrors(const QList<QSslError> &errs)
{
	for(const auto &e:errs)
		qWarning()<<"tcp control socket ssl error: "<<e.errorString()<<"\n";
}

int TcpControlSocket::findClient(QSslSocket *sock)
{
	for(int i=0;i<clients.count();++i)
		if(clients[i].sock==sock)
			return i;
	return -1;
}

void TcpControlSocket::closeClient(QSslSocket *sock)
{
	int index=findClient(sock);
	if(index==-1)
		return;
	ClientSet &set=clients[index];
	if(set.proc)
		set.proc->scheduleDelete();
	else set.sock->deleteLater();
	clients.removeAt(index);
	qInfo()<<"Client disconnected";
}
