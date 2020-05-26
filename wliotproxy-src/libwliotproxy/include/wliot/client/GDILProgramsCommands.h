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

#include "wliot/client/BaseProgramsControlCommands.h"
#include "GDIL/core/BaseBlock.h"
#include "GDIL/core/Program.h"

namespace WLIOTClient
{
	class GDILProgramsCommands
		:public BaseProgramsControlCommands
	{
		Q_OBJECT
	public:
		explicit GDILProgramsCommands(ServerConnection *conn);
		bool listConfigOptions(const QByteArray &id,QList<GDILConfigOption> &options);
		bool setConfigOption(const QByteArray &id,
			const WLIOTGDIL::ConfigOptionId &optionId,const WLIOTGDIL::DataUnit &data);
		bool listTimers(const QByteArray &id,QList<GDILTimer> &timers);
		bool setTimer(const QByteArray &id,quint32 blockId,const WLIOTGDIL::TimerBlock::TimerConfig &cfg);

	private:
		ServerConnection *srvConn;
	};
}

#endif // GDILPROGRAMSCOMMANDS_H
