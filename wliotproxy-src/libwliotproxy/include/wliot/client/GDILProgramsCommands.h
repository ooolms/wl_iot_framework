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

#ifndef GDILPROGRAMSCOMMANDS_H
#define GDILPROGRAMSCOMMANDS_H

#include "wliot/client/ServerConnection.h"
#include "wliot/client/ServerDataTypes.h"
#include "GDIL/core/BaseBlock.h"
#include "GDIL/core/Program.h"

namespace WLIOTClient
{
	class GDILProgramsCommands
		:public QObject
	{
		Q_OBJECT
	public:
		explicit GDILProgramsCommands(ServerConnection *conn);
		bool list(QByteArrayList &programs,QList<bool> &states);
		bool get(const QByteArray &programName,QByteArray &text);
		bool remove(const QByteArray &programName);
		bool upload(const QByteArray &programName,const QByteArray &text);
		bool start(const QByteArray &programName);
		bool stop(const QByteArray &programName);
		bool restart(const QByteArray &programName);
		bool listConfigOptions(const QByteArray &programName,QList<GDILConfigOption> &options);
		bool setConfigOption(const QByteArray &programName,
			const WLIOTGDIL::ConfigOptionId &id,const WLIOTGDIL::DataUnit &data);
		bool listTimers(const QByteArray &programName,QList<GDILTimer> &timers);
		bool setTimer(const QByteArray &programName,quint32 blockId,const WLIOTGDIL::TimerBlock::TimerConfig &cfg);

	private:
		ServerConnection *srvConn;
	};
}

#endif // GDILPROGRAMSCOMMANDS_H
