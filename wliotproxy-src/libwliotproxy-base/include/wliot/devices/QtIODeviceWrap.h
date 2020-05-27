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

#ifndef QTIODEVICEWRAP_H
#define QTIODEVICEWRAP_H

#include "wliot/devices/StreamParser.h"
#include <QObject>
#include <QIODevice>
#include <QUuid>
#include <functional>

namespace WLIOT
{
	/**
	 * @brief The QtIODeviceWrap class
	 * Данный класс реализует протокол обмена сообщениями WLIOT поверх какого-то устройства (QIODevice).
	 */
	class QtIODeviceWrap
		:public QObject
	{
		Q_OBJECT
	public:
		typedef std::function<void()> OnDataWritten;

	public:
		explicit QtIODeviceWrap(QIODevice *d,OnDataWritten onDataWrittenFunc=0,QObject *parent=0);
		virtual ~QtIODeviceWrap();
		bool writeMsg(const Message &m);
		bool writeMsg(const QByteArray &m);
		bool writeMsg(const QByteArray &m,const QByteArrayList &args);
		bool isConnected();
		QIODevice* ioDev();

	public slots:
		void onDeviceConnected();
		void onDeviceDisconnected();
		void readReadyData();

	signals:
		void disconnected();
		void connected();
		void newMessage(const WLIOT::Message &m);
		void streamWasReset();

	private slots:
		void onDeviceDestroyed();
		void onDataReady();

	private:
		bool writeData(const QByteArray &data);

	private:
		QIODevice *dev;
		bool mIsConnected;
		OnDataWritten mOnDataWritten;
		StreamParser streamParser;
	};
}

#endif // QTIODEVICEWRAP_H
