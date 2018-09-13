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

#ifndef ARPCSERIALDRIVER_H
#define ARPCSERIALDRIVER_H

#include <QObject>
#include <QFile>
#include <QThread>
#include <QTimer>
#include <QMutex>

#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <errno.h>
#include <signal.h>
#endif

#ifdef Q_OS_WIN
	typedef HANDLE FileDescrType;
#else
	typedef int FileDescrType;
#endif

class ARpcCommReader
	:public QThread
{
	Q_OBJECT
public:
	explicit ARpcCommReader(FileDescrType f,QObject *parent=0);
	virtual ~ARpcCommReader();
	void writeData(const QByteArray &data);

signals:
	void newData(const QByteArray &rData);
	void readError();
	void writeError();

protected:
	virtual void run();

private:
	void writePendingData();
	void onTimer();

private:
	QTimer *t;
	QMutex m;
	FileDescrType fd;
	QByteArray rData,wData;
#ifdef Q_OS_WIN
	OVERLAPPED rs,ws;
#else
#endif
};

class ARpcSerialDriver
	:public QObject
{
	Q_OBJECT
public:
	enum Error
	{
		NoError,
		AccessError,
		ReadError,
		WriteError,
		UnknownError
	};

public:
	explicit ARpcSerialDriver(const QString &portName,QObject *parent=0);
	virtual ~ARpcSerialDriver();
	bool open();
	bool isOpened();
	void close();
	Error errorCode();
	QString errorString();
	void write(const QByteArray &data);
	QString portName();
	void startReader();

signals:
	void newData(const QByteArray &data);
	void error();

private:
	void setupSerialPort();

private:
	QString mPortName;
//	QFile mFile;
	Error lastError;
	FileDescrType fd;
	ARpcCommReader *reader;
};

#endif // ARPCSERIALDRIVER_H
