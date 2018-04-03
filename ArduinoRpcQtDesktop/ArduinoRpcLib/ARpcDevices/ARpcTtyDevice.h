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

#ifndef ARPCTTYDEVICE_H
#define ARPCTTYDEVICE_H

#include "ARpcBase/ARpcStreamParser.h"
#include "ARpcBase/ARpcRealDevice.h"
#include <QFileSystemWatcher>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>

class ARpcTtyDevice
	:public ARpcRealDevice
{
	Q_OBJECT
public:
	explicit ARpcTtyDevice(const QString &portName,QObject *parent=0);
	virtual ~ARpcTtyDevice();
	virtual bool writeMsg(const ARpcMessage &m)override;
	virtual bool isConnected()override;
	QString portName()const;

public://tty port settings
	void setBaudRate(qint32 rate, QSerialPort::Directions directions=QSerialPort::AllDirections);
	void setDataBits(QSerialPort::DataBits bits);
	void setFlowControl(QSerialPort::FlowControl ctl);
	void setParity(QSerialPort::Parity parity);
	void setStopBits(QSerialPort::StopBits bits);
	qint32 baudRate();
	QSerialPort::DataBits dataBits();
	QSerialPort::FlowControl flowControl();
	QSerialPort::Parity parity();
	QSerialPort::StopBits stopBits();

public slots:
	void tryOpen();

private slots:
//	void onWatcherFileChanged(const QString &filePath);
//	void onWatcherDirChanged(const QString &dirPath);
	void onReadyRead();
	void onPortError(QSerialPort::SerialPortError err);

private:
	void closeTty();
	void setupSerialPort();

private:
//	QFileSystemWatcher watcher;
	QSerialPortInfo info;
	QTimer reconnectTimer;
//	int fd;
//	QFile *file;
//	QSocketNotifier *notif;
	QSerialPort *ttyPort;
};

#endif // ARPCTTYDEVICE_H
