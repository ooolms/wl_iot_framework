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

#ifndef ARPCOUTSIDEDEVICE_H
#define ARPCOUTSIDEDEVICE_H

#include "ARpcBase/ARpcMessage.h"
#include "ARpcBase/ARpcStreamParser.h"
#include "ARpcBase/ARpcMessageParser.h"
#include <QObject>
#include <QIODevice>
#include <QUuid>

class ARpcOutsideDevice
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcOutsideDevice(QIODevice *d,QObject *parent=0);
	virtual ~ARpcOutsideDevice(){}
	bool writeMsg(const QString &msg);
	bool writeMsg(const QString &msg,const QStringList &args);
	bool writeMsg(const ARpcMessage &m);

signals:
	void disconnected();
	void connected();
	void rawMessage(const ARpcMessage &m);

private slots:
	void onDeviceDestroyed();
	void onDataReady();

protected://для потомков
	ARpcMessageParser msgParser;//использовать dump для реализации writeMsg
	ARpcStreamParser streamParser;//совать туда поток байт от устройства

private:
	QIODevice *dev;
};

#endif // ARPCOUTSIDEDEVICE_H
