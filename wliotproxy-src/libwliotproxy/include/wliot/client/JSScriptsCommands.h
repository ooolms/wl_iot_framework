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

#ifndef JSSCRIPTSCOMMANDS_H
#define JSSCRIPTSCOMMANDS_H

#include <QObject>
#include "wliot/client/ServerConnection.h"

namespace WLIOTClient
{
	class JSScriptsCommands
		:public QObject
	{
		Q_OBJECT
	public:
		explicit JSScriptsCommands(ServerConnection *conn);
		bool list(QByteArrayList &scripts,QList<bool> &states);
		bool get(const QByteArray &scriptName,QByteArray &text);
		bool remove(const QByteArray &scriptName);
		bool upload(const QByteArray &scriptName,const QByteArray &text);
		bool start(const QByteArray &scriptName);
		bool stop(const QByteArray &scriptName);
		bool restart(const QByteArray &scriptName);

	private:
		ServerConnection *srvConn;
	};
}

#endif // JSSCRIPTSCOMMANDS_H
