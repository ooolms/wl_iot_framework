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

#ifndef SERIALDRIVER_H
#define SERIALDRIVER_H

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
#include <termios.h>
#endif

namespace WLIOT
{
	#ifdef Q_OS_WIN
		typedef HANDLE FileDescrType;
	#else
		typedef int FileDescrType;
	#endif

	class CommReader
		:public QThread
	{
		Q_OBJECT
	public:
		explicit CommReader(FileDescrType f,QObject *parent=0);
		virtual ~CommReader();
		void writeData(const QByteArray &data);

	signals:
		void newData(QByteArray rData);
		void readError();
		void writeError();

	protected:
		virtual void run();

	private slots:
		void onTimer();

	private:
		void writePendingData();

	private:
		QMutex m;
		FileDescrType fd;
		QByteArray rData,wData;
	#ifdef Q_OS_WIN
		OVERLAPPED rs,ws;
	#else
		int writeEventFd;
	#endif
	};

	class SerialDriver
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

	private:
		enum BoudRate
		{
#ifdef Q_OS_WIN
			BRate300=300,
			BRate600=600,
			BRate1200=1200,
			BRate2400=2400,
			BRate4800=4800,
			BRate9600=9600,
			BRate19200=19200,
			BRate38400=38400,
			BRate57600=57600,
			BRate115200=115200,
			BRate230400=230400,
			BRate460800=460800,
			BRate921600=921600,
			BRate1500000=1500000,
			BRate2000000=2000000,
			BRate2500000=2500000,
			BRate3000000=3000000,
			BRate3500000=3500000,
			BRate4000000=4000000
#else
			BRate300=B300,
			BRate600=B600,
			BRate1200=B1200,
			BRate2400=B2400,
			BRate4800=B4800,
			BRate9600=B9600,
			BRate19200=B19200,
			BRate38400=B38400,
			BRate57600=B57600,
			BRate115200=B115200,
			BRate230400=B230400,
			BRate460800=B460800,
			BRate921600=B921600,
			BRate1500000=B1500000,
			BRate2000000=B2000000,
			BRate2500000=B2500000,
			BRate3000000=B3000000,
			BRate3500000=B3500000,
			BRate4000000=B4000000
#endif
		};

		enum FlowControl
		{
			NoFlowControl,
			SwFlowControl,
			HwFlowControl
		};

	public:
		/**
		 * @brief SerialDriver
		 * класс, выполняющий операции с последовательным портом: настройка, открытие, чтение/запись и т.д.
		 * @param portName имя порта (например, ttyACM0 или COM19)
		 * @param connOptions параметры соединения, строка вида baudRate:dataBits:stopBits:parity:flowControl
		 * значения начиная с конца могут быть опущены, например 9600:8:1 или 4800:6:2:odd
		 * baudRate - 300, 600, 1200,2400, 4800, 9600 (default), 19200, 38400, 57600, 115200, 230400, 460800, 921600,
		 *    1500000, 2000000, 2500000, 3000000, 3500000, 4000000
		 * dataBits - 5, 6, 7, 8(default)
		 * stopBits - 1 (default), 2
		 * parity - no, odd, even, mark, space
		 * flowControl - no (defalut), hw (RTS/CTS), sw(XON/XOFF)
		 * @param parent
		 */
		explicit SerialDriver(const QString &portName,const QString &connOptions,QObject *parent=0);
		virtual ~SerialDriver();
		bool open();
		bool isOpened();
		void close();
		Error errorCode();
		QString errorString();
		void write(const QByteArray &data);
		QString portName();
		void startReader();

	signals:
		void newData(QByteArray data);
		void error();

	private:
		void setupSerialPort();
		void setBRate(quint32 valueFromStr);
		void setDataBits(quint8 valueFromStr);
		void setStopBits(quint8 valueFromStr);
		void setParity(const QString &str);
		void setFlowControl(const QString &str);

	private:
		BoudRate bRate;
#ifdef Q_OS_WIN
		BYTE dataBits;
		BYTE stopBits;
		BYTE parity;
#else
		tcflag_t dataBitsFlag;//to cflag
		tcflag_t stopBitsFlag;//to iflag
		tcflag_t parityFlag;//to cflag
#endif
		FlowControl flowControl;
		QString mPortName;
	//	QFile mFile;
		Error lastError;
		FileDescrType fd;
		CommReader *reader;
	};
}

#endif // SERIALDRIVER_H
