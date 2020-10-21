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

#ifndef BASEPROGRAMSCONTROLCOMMANDS_H
#define BASEPROGRAMSCONTROLCOMMANDS_H

#include "wliot/client/ServerConnection.h"

namespace WLIOTClient
{
	class BaseProgramsControlCommands
		:public QObject
	{
		Q_OBJECT
	public:
		explicit BaseProgramsControlCommands(const QByteArray &cmdPrefix,ServerConnection *conn);
		virtual ~BaseProgramsControlCommands(){}
		bool list(QByteArrayList &ids,QByteArrayList &names,QList<bool> &states);
		bool get(const QByteArray &id,QByteArray &text);
		bool remove(const QByteArray &id);
		bool rename(const QByteArray &id,const QByteArray &name);
		bool create(const QByteArray &name,const QByteArray &data,QByteArray &id);
		bool update(const QByteArray &id,const QByteArray &data);
		bool start(const QByteArray &id);
		bool stop(const QByteArray &id);
		bool restart(const QByteArray &id);

	private:
		ServerConnection *srvConn;
		QByteArray mCmdPrefix;
	};
}

#endif // BASEPROGRAMSCONTROLCOMMANDS_H
