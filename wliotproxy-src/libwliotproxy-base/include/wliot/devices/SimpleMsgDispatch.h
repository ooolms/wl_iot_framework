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

#ifndef SIMPLEMSGDISPATCH_H
#define SIMPLEMSGDISPATCH_H

#include "wliot/devices/Message.h"
#include <QObject>

namespace WLIOT
{
	class SimpleMsgDispatch
		:public QObject
	{
		Q_OBJECT
	public:
		explicit SimpleMsgDispatch(QObject *parent=0);

	signals:
		void infoMsg(const QByteArray &str);
		void measurementMsg(const QByteArray &sensor,const QByteArray &value);
		void commandStateChanged(const QByteArray &command,int index,const QByteArray &value);
		void additionalStateChanged(const QByteArray &key,const QByteArray &value);

	public slots:
		void onNewMessageFromDevice(const Message &m);
	};
}

#endif // ARPCSIMPLEMSGDISPATCH_H
