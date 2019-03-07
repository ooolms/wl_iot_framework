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

#ifndef ARPCTCPDEVICEDETECT_H
#define ARPCTCPDEVICEDETECT_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include "ARpcTcpDeviceDetectServer.h"
#include "wliot/devices/Message.h"

class ARpcTcpDeviceDetect
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcTcpDeviceDetect(QObject *parent=0);
	bool isServerListening();
	void startRegularBroadcasting(int msecsDelay);
	void stopRegularBroadcasting();

public slots:
	void broadcastServerReadyMessage();

signals:
	void newClient(qintptr s,bool &accepted);

private:
	ARpcTcpDeviceDetectServer srv;
	QUdpSocket bCastSock;
	QByteArray bCastMsg;
	QTimer tmr;
};

#endif // ARPCTCPDEVICEDETECT_H
