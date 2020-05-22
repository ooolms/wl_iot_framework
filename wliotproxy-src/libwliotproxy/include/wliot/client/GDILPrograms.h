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

#ifndef GDILPROGRAMS_H
#define GDILPROGRAMS_H

#include "wliot/client/IGDILProgramsManager.h"
#include "wliot/client/ServerConnection.h"
#include "wliot/client/AllServerCommands.h"

namespace WLIOTClient
{
	class GDILPrograms
		:public IGDILProgramsManager
	{
		Q_OBJECT
	public:
		explicit GDILPrograms(ServerConnection *conn,AllServerCommands *cmds);
		virtual QByteArrayList programs()override;
		virtual bool get(const QByteArray &programName,QByteArray &text)override;
		virtual bool isWorking(const QByteArray &programName)override;
		virtual bool setProgramXml(const QByteArray &programName,const QByteArray &xml)override;
		virtual bool remove(const QByteArray &programName)override;
		virtual void start(const QByteArray &programName)override;
		virtual void stop(const QByteArray &programName)override;
		virtual void restart(const QByteArray &programName)override;
		virtual bool listConfigOptions(const QByteArray &programName,QList<GDILConfigOption> &options)override;
		virtual bool setConfigOption(const QByteArray &programName,
			const WLIOTGDIL::ConfigOptionId &id,const WLIOTGDIL::DataUnit &data)override;
		virtual bool listTimers(const QByteArray &programName,QList<GDILTimer> &timers)override;
		virtual bool setTimer(const QByteArray &programName,quint32 blockId,
			const WLIOTGDIL::TimerBlock::TimerConfig &cfg)override;
		bool reloadPrograms();

	private slots:
		void onConnected();
		void onDisconnected();

	private:
		ServerConnection *srvConn;
		AllServerCommands *srvCmds;
		QMap<QByteArray,bool> programsMap;
		bool ready;
	};
}

#endif // GDILPROGRAMS_H
