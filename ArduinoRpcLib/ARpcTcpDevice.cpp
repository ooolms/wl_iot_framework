#include "ARpcTcpDevice.h"

ARpcTcpDevice::ARpcTcpDevice(const QHostAddress &addr,const ARpcConfig &cfg,QObject *parent)
	:ARpcDevice(cfg,parent)
{
	address=addr;
	port=4081;
	retryTimer.setInterval(60*1000);
	retryTimer.setSingleShot(false);

	connect(&retryTimer,&QTimer::timeout,this,&ARpcTcpDevice::onRetryTimer);
	connect(&socket,&QSslSocket::connected,this,&ARpcTcpDevice::onSocketConnected);
	connect(&socket,&QSslSocket::disconnected,this,&ARpcTcpDevice::onSocketDisonnected);

	retryTimer.start();
	onRetryTimer();
}

bool ARpcTcpDevice::isConnected()
{
	//TODO if ssl, check if encrypted
	return socket.state()==QAbstractSocket::ConnectedState;
}

void ARpcTcpDevice::onRetryTimer()
{
	if(socket.state()!=QAbstractSocket::UnconnectedState)return;
	socket.connectToHost(address,port);
}

void ARpcTcpDevice::onSocketConnected()
{
	emit connected();
	retryTimer.stop();
}

void ARpcTcpDevice::onSocketDisonnected()
{
	emit disconnected();
	retryTimer.start();
}
