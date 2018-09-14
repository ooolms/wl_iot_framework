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

#include "ARpcBase/ARpcDevice.h"
#include <QObject>
#include <QIODevice>
#include <QUuid>
#include <functional>

/**
 * @brief The ARpcOutsideDevice class
 * Устройство без идентификатора, имени, сенсоров, описания и т.д.
 * Фактически, данный класс только реализует протокол обмена сообщениями ARpc поверх какого-то устройства (QIODevice).
 */
class ARpcOutsideDevice
	:public ARpcDevice
{
	Q_OBJECT
public:
	typedef std::function<void()> OnDataWritten;

public:
	explicit ARpcOutsideDevice(QIODevice *d,OnDataWritten onDataWrittenFunc=0,QObject *parent=0);
	virtual ~ARpcOutsideDevice(){}
	bool writeMsg(const QByteArray &msg);
	bool writeMsg(const QByteArray &msg,const QByteArrayList &args);
	virtual bool writeMsg(const ARpcMessage &m)override;
	virtual bool isConnected()override;
	QIODevice* ioDev();

public slots:
	void onDeviceConnected();
	void onDeviceDisconnected();
	void readReadyData();

private slots:
	void onDeviceDestroyed();
	void onDataReady();

private:
	QIODevice *dev;
	bool mIsConnected;
	OnDataWritten mOnDataWritten;
};

#endif // ARPCOUTSIDEDEVICE_H
