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

#ifndef ARPCTCPDEVICEBACKEND_H
#define ARPCTCPDEVICEBACKEND_H

#include "wliot/devices/ILowLevelDeviceBackend.h"
#include <QObject>
#include <QHostAddress>
#include <QTcpSocket>
#include <QTimer>

namespace WLIOT
{
	class TcpDeviceBackend
		:public ILowLevelDeviceBackend
	{
		Q_OBJECT
	public:
		explicit TcpDeviceBackend(const QString &addr,const QString &connString,QObject *parent=nullptr);
		explicit TcpDeviceBackend(qintptr s,const QString &addr,quint16 port,QObject *parent=nullptr);
		virtual bool waitForConnected(int msecs)override;
		bool writeData(const QByteArray &data)override;
		bool flush()override;
		bool isConnected()const override;
		void forceDisconnect()override;
		virtual QByteArray backendType()const override;
		static void readAddrFromSocket(qintptr s,QString &address,quint16 &port);

	protected://for ssl socket
		/**
		 * @brief TcpDeviceBackend
		 * не создает сокет, нужен для наследника - TcpSslDeviceBackend
		 */
		explicit TcpDeviceBackend(const QString &addr,const QString &connString,quint16 port,QObject *parent=nullptr);
		virtual void startSocketConnection();
		virtual void processOnSocketConnected();
		void setupSocket();

	private slots:
		void onSocketConnected();
		void onSocketDisonnected();
		void onReadyRead();

	public:
		static const QByteArray mBackendType;

	protected:
		QString mAddress;
		quint16 mPort;
		QTcpSocket *mSocket;
	};
}

#endif // ARPCTCPDEVICEBACKEND_H
