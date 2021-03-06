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

#ifndef SERIALDEVICEBACKEND_H
#define SERIALDEVICEBACKEND_H

#include "wliot/devices/ILowLevelDeviceBackend.h"
#include <QFileSystemWatcher>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QSocketNotifier>
#include <QFile>
#include <QTimer>

namespace WLIOT
{
	class SerialDriver;

	class SerialDeviceBackend
		:public ILowLevelDeviceBackend
	{
		Q_OBJECT
	public:
		explicit SerialDeviceBackend(const QString &portName,
			const QString &connOptions=QString(),QObject *parent=nullptr);
		virtual ~SerialDeviceBackend();
		virtual bool writeData(const QByteArray &data)override;
		virtual bool flush()override;
		virtual bool isConnected()const override;
		virtual void forceDisconnect()override;
		virtual QByteArray backendType()const override;
		virtual bool waitForConnected(int msecs)override;
		void tryOpen();

	private slots:
		void onNewData(const QByteArray &data);
		void onPortError();
		void onDevDirChanged();

	private:
		void closeTty();
		void setupSerialPort();

	public:
		static const QByteArray mBackendType;

	private:
		SerialDriver *ttyPort;
	};
}

#endif // SERIALDEVICEBACKEND_H
