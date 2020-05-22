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

#include <QObject>
#include <QByteArray>
#include "GDIL/core/Program.h"
#include "wliot/client/ServerDataTypes.h"

namespace WLIOTClient
{
	class IGDILProgramsManager
		:public QObject
	{
		Q_OBJECT
	public:
		explicit IGDILProgramsManager(QObject *parent=nullptr);
		virtual QByteArrayList programs()=0;
		virtual bool get(const QByteArray &programName,QByteArray &text)=0;
		virtual bool isWorking(const QByteArray &programName)=0;
		virtual bool setProgramXml(const QByteArray &programName,const QByteArray &xml)=0;
		virtual bool remove(const QByteArray &programName)=0;
		virtual void start(const QByteArray &programName)=0;
		virtual void stop(const QByteArray &programName)=0;
		virtual void restart(const QByteArray &programName)=0;
		virtual bool listConfigOptions(const QByteArray &programName,QList<GDILConfigOption> &options)=0;
		virtual bool setConfigOption(const QByteArray &programName,
			const WLIOTGDIL::ConfigOptionId &id,const WLIOTGDIL::DataUnit &data)=0;
		virtual bool listTimers(const QByteArray &programName,QList<GDILTimer> &timers)=0;
		virtual bool setTimer(const QByteArray &programName,
			quint32 blockId,const WLIOTGDIL::TimerBlock::TimerConfig &cfg)=0;

	signals:
		void stateChanged(const QByteArray &programName,bool isWorking);
	};
}

#endif // IGDILPROGRAMSMANAGER_H
