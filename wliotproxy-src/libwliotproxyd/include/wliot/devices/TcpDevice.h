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

#include "wliot/devices/StreamParser.h"
#include "wliot/devices/RealDevice.h"
#include <QObject>
#include <QHostAddress>
#include <QTcpSocket>
#include <QTimer>

class TcpDevice
	:public RealDevice
{
	Q_OBJECT
public:
	explicit TcpDevice(const QString &addr,QObject *parent=0);
	explicit TcpDevice(qintptr s,QObject *parent=0);
	virtual void setNewSocket(qintptr s,const QUuid &newId=QUuid(),const QByteArray &newName=QByteArray());
	virtual bool writeMsgToDevice(const Message &m)override;
	QString address()const;
	qintptr socketDescriptor();
	virtual bool waitForConnected();
	void disconnectFromHost();

public://use this ONLY to move socket between two QTcpDevices
	QTcpSocket* takeSocket();
	void setNewSocket(QTcpSocket *s,const QUuid &newId=QUuid(),const QByteArray &newName=QByteArray());

protected:
	explicit TcpDevice(QObject *parent=0);
	void setupSocket();
	void readAddrFromSocket(qintptr s);
	virtual void syncFailed();
	virtual void startSocketConnection();
	virtual void processOnSocketConnected();

private slots:
	void onSocketConnected();
	void onSocketDisonnected();
	void onReadyRead();
	void onSocketDestroyed();

protected:
	QString mAddress;
	quint16 mPort;
	QTcpSocket *mSocket;
	StreamParser streamParser;
};

#endif // ARPCTCPDEVICE_H
