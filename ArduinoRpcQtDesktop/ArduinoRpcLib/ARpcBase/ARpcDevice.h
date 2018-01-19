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

#ifndef ARPCDEVICE_H
#define ARPCDEVICE_H

#include "ARpcBase/ARpcMessage.h"
#include "ARpcBase/ARpcStreamParser.h"
#include "ARpcBase/ARpcSensor.h"
#include "ARpcBase/ARpcControlsDefinition.h"
#include "ARpcBase/ARpcDeviceState.h"
#include <QObject>
#include <QUuid>
#include <QTimer>

class ARpcDevice
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcDevice(QObject *parent=0);
	virtual ~ARpcDevice(){}
	bool writeMsg(const QString &msg);
	bool writeMsg(const QString &msg,const QStringList &args);
	ARpcIMessageHandler* setMessageCHandler(ARpcIMessageHandler *h);
	ARpcStreamParser::MessageHandler setMessageFHandler(ARpcStreamParser::MessageHandler h);

public:
	virtual bool writeMsg(const ARpcMessage &m)=0;
	virtual bool isConnected()=0;

signals:
	void disconnected();
	void connected();
	void rawMessage(const ARpcMessage &m);

protected://для потомков
	ARpcStreamParser streamParser;//совать туда поток байт от устройства, использовать dump для реализации writeMsg
};

#endif // ARPCDEVICE_H
