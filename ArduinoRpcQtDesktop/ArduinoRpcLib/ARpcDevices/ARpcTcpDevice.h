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

#ifndef ARPCTCPDEVICE_H
#define ARPCTCPDEVICE_H

#include "ARpcBase/ARpcStreamParser.h"
#include "ARpcBase/ARpcMessageParser.h"
#include "ARpcBase/ARpcRealDevice.h"
#include <QObject>
#include <QHostAddress>
#include <QSslSocket>
#include <QTimer>

//tcp port 4081
//TODO ssl support
class ARpcTcpDevice
	:public ARpcRealDevice
{
	Q_OBJECT
public:
	explicit ARpcTcpDevice(const QHostAddress &addr,QObject *parent=0);
	virtual bool writeMsg(const ARpcMessage &m)override;
	virtual bool isConnected()override;
	QHostAddress getAddress()const;

private slots:
	void onRetryTimer();
	void onSocketConnected();
	void onSocketDisonnected();

public:
	static const quint16 stdDevPort;

private:
	QHostAddress address;
	QSslSocket socket;
	quint16 port;
	QTimer retryTimer;
};

#endif // ARPCTCPDEVICE_H
