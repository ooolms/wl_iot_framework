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
#include "ARpcBase/ARpcMessageParser.h"
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
	bool isIdentified();
	QUuid id();
	QString name();//human-readable
	bool getSensorsDescription(QList<ARpcSensor> &sensors);
	bool getControlsDescription(ARpcControlsGroup &controls);
	bool getState(ARpcDeviceState &state);

public:
	virtual bool writeMsg(const ARpcMessage &m)=0;
	virtual bool isConnected()=0;

public slots:
	bool identify();

signals:
	void disconnected();
	void connected();
	void rawMessage(const ARpcMessage &m);
	void identificationChanged();

protected:
	void resetIdentification();

private slots:
	void onDisconnected();

private:
	bool internallFunctionCall(const ARpcMessage &msg,
		const ARpcMessage &estimatedReturnMsg,int timeout,QStringList &retVal);
		//timeout: msecs, estimated msg: check title and first args of returned msg

protected://для потомков
	ARpcMessageParser msgParser;//использовать dump для реализации writeMsg
	ARpcStreamParser streamParser;//совать туда поток байт от устройства
	QUuid devId;
	QString devName;

private:
	QTimer identifyTimer;
};

#endif // ARPCDEVICE_H
