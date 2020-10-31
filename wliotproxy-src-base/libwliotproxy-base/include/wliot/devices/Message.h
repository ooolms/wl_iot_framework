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

#ifndef MESSAGE_H
#define MESSAGE_H

#include <QByteArray>
#include <QList>
#include <QMetaType>
#include <QEvent>

namespace WLIOT
{
	class Message
	{
	public:
		Message();
		Message(const QByteArray &t);
		Message(const QByteArray &t,const QByteArrayList &a);
		static QByteArray escape(const QByteArray &data);
		QByteArray dump()const;
		static QByteArray dump(const QByteArray &msg);
		static QByteArray dump(const QByteArray &msg,const QByteArrayList &args);

	public:
		QByteArray title;
		QByteArrayList args;
	};

	class MessageEvent
		:public QEvent
	{
	public:
		explicit MessageEvent(const Message &m);
		const Message& msg();

	public:
		static const int type;

	private:
		Message m;
	};

}

Q_DECLARE_METATYPE(WLIOT::Message)

#endif // MESSAGE_H
