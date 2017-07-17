#include "ARpcTcpDevice.h"

const quint16 ARpcTcpDevice::stdDevPort=4081;

ARpcTcpDevice::ARpcTcpDevice(const QHostAddress &addr,QObject *parent)
	:ARpcDevice(parent)
{
	address=addr;
	port=stdDevPort;
	retryTimer.setInterval(60*1000);
	retryTimer.setSingleShot(false);

	connect(&retryTimer,&QTimer::timeout,this,&ARpcTcpDevice::onRetryTimer);
	connect(&socket,&QSslSocket::connected,this,&ARpcTcpDevice::onSocketConnected);
	connect(&socket,&QSslSocket::disconnected,this,&ARpcTcpDevice::onSocketDisonnected);

	retryTimer.start();
	onRetryTimer();
	socket.waitForConnected();
}

bool ARpcTcpDevice::writeMsg(const ARpcMessage &m)
{
	if(socket.state()!=QAbstractSocket::ConnectedState)return false;
	QByteArray data=(msgParser.dump(m)+ARpcConfig::msgDelim).toUtf8();
	return socket.write(data)==data.size();
}

bool ARpcTcpDevice::isConnected()
{
	//TODO if ssl, check if encrypted
	return socket.state()==QAbstractSocket::ConnectedState;
}

QHostAddress ARpcTcpDevice::getAddress()const
{
	return address;
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
	resetIdentification();
	retryTimer.start();
}
