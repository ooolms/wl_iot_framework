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
#include <QTcpSocket>
#include <QTimer>

class ARpcTcpDevice
	:public ARpcRealDevice
{
	Q_OBJECT
public:
	explicit ARpcTcpDevice(const QHostAddress &addr,QObject *parent=0);
	explicit ARpcTcpDevice(QTcpSocket *s,QObject *parent=0);
	void setNewSocket(QTcpSocket *s,const QUuid &newId=QUuid(),const QString &newName=QString());
	virtual bool writeMsg(const ARpcMessage &m)override;
	virtual bool isConnected()override;
	QHostAddress address()const;

private slots:
	void onReconnectTimer();
	void onSocketConnected();
	void onSocketDisonnected();
	void onReadyRead();

private:
	QHostAddress mAddress;
	QTcpSocket *socket;
	QTimer reconnectTimer;
};

#endif // ARPCTCPDEVICE_H
