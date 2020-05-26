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

#ifndef IGDILPROGRAMSMANAGER_H
#define IGDILPROGRAMSMANAGER_H

#include "wliot/client/IBaseProgramsManager.h"
#include <QObject>
#include "GDIL/core/Program.h"
#include "wliot/client/ServerDataTypes.h"

namespace WLIOTClient
{
	class IGDILProgramsManager
		:public QObject
		,public IBaseProgramsManager
	{
		Q_OBJECT
	public:
		explicit IGDILProgramsManager(QObject *parent=nullptr);
		virtual bool listConfigOptions(const QByteArray &id,QList<GDILConfigOption> &options)=0;
		virtual bool setConfigOption(const QByteArray &id,
			const WLIOTGDIL::ConfigOptionId &optionId,const WLIOTGDIL::DataUnit &data)=0;
		virtual bool listTimers(const QByteArray &id,QList<GDILTimer> &timers)=0;
		virtual bool setTimer(const QByteArray &id,
			quint32 blockId,const WLIOTGDIL::TimerBlock::TimerConfig &cfg)=0;
	};
}

#endif // IGDILPROGRAMSMANAGER_H
