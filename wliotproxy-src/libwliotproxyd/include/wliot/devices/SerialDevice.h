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

#include "wliot/devices/StreamParser.h"
#include "wliot/devices/RealDevice.h"
#include "wliot/devices/StreamParser.h"
#include <QFileSystemWatcher>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QSocketNotifier>
#include <QFile>
#include <QTimer>

class SerialDriver;

class SerialDevice
	:public RealDevice
{
	Q_OBJECT
public:
	explicit SerialDevice(const QString &portName,QObject *parent=0);
	virtual ~SerialDevice();
	virtual bool writeMsgToDevice(const Message &m)override;
	QString portName()const;
	virtual QByteArray deviceType(){return "tty";}

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

public slots:
	void tryOpen();

protected:
	virtual void syncFailed();

private slots:
//	void onWatcherFileChanged(const QString &filePath);
//	void onWatcherDirChanged(const QString &dirPath);
	void onNewData(const QByteArray &data);
	void onPortError();
	void onDevDirChanged();

private:
	void closeTty();
	void setupSerialPort();

private:
	QFileSystemWatcher watcher;
	QTimer reconnectTimer;
	StreamParser streamParser;
//	int fd;
//	QFile *file;
//	QSocketNotifier *notif;
//	QSerialPortInfo info;
//	QSerialPort *ttyPort;
	SerialDriver *ttyPort;
};

#endif // ARPCSERIALDEVICE_H
