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

#ifndef ARPCTCPSSLDEVICE_H
#define ARPCTCPSSLDEVICE_H

#include "wliot/devices/StreamParser.h"
#include "wliot/devices/TcpDevice.h"
#include <QObject>
#include <QHostAddress>
#include <QSslSocket>
#include <QTimer>

class TcpSslDevice
	:public TcpDevice
{
	Q_OBJECT
public:
	explicit TcpSslDevice(const QString &addr,QObject *parent=0);
	explicit TcpSslDevice(qintptr s,QObject *parent=0);
	virtual void setNewSocket(qintptr s,const QUuid &newId=QUuid(),const QByteArray &newName=QByteArray())override;
	virtual bool waitForConnected()override;

public://use this ONLY to move socket between two QTcpDevices
	void setNewSocket(QSslSocket *s,const QUuid &newId=QUuid(),const QByteArray &newName=QByteArray());

protected:
	virtual void startSocketConnection()override;
	virtual void processOnSocketConnected()override;

private slots:
	void onSocketEncrypted();
	void onSslErrors();

private:
	inline QSslSocket* sslSocket(){return ((QSslSocket*)mSocket);}
};

#endif // ARPCTCPSSLDEVICE_H
