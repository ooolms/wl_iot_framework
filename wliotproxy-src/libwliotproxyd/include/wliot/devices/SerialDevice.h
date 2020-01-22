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

#ifndef SERIALDEVICE_H
#define SERIALDEVICE_H

#include "wliot/devices/ILowLevelDeviceBackend.h"
#include <QFileSystemWatcher>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QSocketNotifier>
#include <QFile>
#include <QTimer>

class SerialDriver;

class SerialDeviceBackend
	:public ILowLevelDeviceBackend
{
	Q_OBJECT
public:
	explicit SerialDeviceBackend(const QString &portName,QObject *parent=nullptr);
	virtual ~SerialDeviceBackend();
	QString portName()const;
	void closeTty();
	void setBaudRate(quint32 r);
	virtual bool writeData(const QByteArray &data)override;
	virtual bool flush()override;
	virtual bool isConnected()const override;
	virtual void forceDisconnect()override;
	void tryOpen();

//public://tty port settings
//	void setBaudRate(qint32 rate, QSerialPort::Directions directions=QSerialPort::AllDirections);
//	void setDataBits(QSerialPort::DataBits bits);
//	void setFlowControl(QSerialPort::FlowControl ctl);
//	void setParity(QSerialPort::Parity parity);
//	void setStopBits(QSerialPort::StopBits bits);
//	qint32 baudRate();
//	QSerialPort::DataBits dataBits();
//	QSerialPort::FlowControl flowControl();
//	QSerialPort::Parity parity();
//	QSerialPort::StopBits stopBits();

private slots:
	void onNewData(const QByteArray &data);
	void onPortError();
	void onDevDirChanged();

private:
	void setupSerialPort();

private:
	SerialDriver *ttyPort;
};

#endif // ARPCSERIALDEVICE_H
