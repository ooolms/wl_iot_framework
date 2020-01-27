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

#ifndef TCPSSLDEVICEBACKEND_H
#define TCPSSLDEVICEBACKEND_H

#include "wliot/devices/TcpDeviceBackend.h"
#include <QObject>
#include <QHostAddress>
#include <QSslSocket>
#include <QTimer>

class TcpSslDeviceBackend
	:public TcpDeviceBackend
{
	Q_OBJECT
public:
	explicit TcpSslDeviceBackend(const QString &addr,QObject *parent=nullptr);
	explicit TcpSslDeviceBackend(qintptr s,QObject *parent=nullptr);
	virtual bool waitForConnected(int msecs=30000)override;
	bool isConnected()const override;
	virtual QByteArray type()const override;

protected:
	virtual void startSocketConnection()override;
	virtual void processOnSocketConnected()override;

private slots:
	void onSocketEncrypted();
	void onSslErrors();

public:
	static const QByteArray devType;

private:
	inline QSslSocket* sslSocket(){return ((QSslSocket*)mSocket);}
	inline const QSslSocket* sslSocket()const{return ((QSslSocket*)mSocket);}
};

#endif // TCPSSLDEVICEBACKEND_H
