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

#ifndef IOTSERVERGDILPROGRAMS_H
#define IOTSERVERGDILPROGRAMS_H

#include "IotServerIGDILProgramsManager.h"
#include "IotServerConnection.h"
#include "IotServerCommands.h"

class IotServerGDILPrograms
	:public IotServerIGDILProgramsManager
{
	Q_OBJECT
public:
	explicit IotServerGDILPrograms(IotServerConnection *conn,IotServerCommands *cmds);
	virtual QByteArrayList programs()override;
	virtual bool get(const QByteArray &programName,QByteArray &text)override;
	virtual bool isWorking(const QByteArray &programName)override;
	virtual bool setProgramXml(const QByteArray &programName,const QByteArray &xml)override;
	virtual bool remove(const QByteArray &programName)override;
	virtual void start(const QByteArray &programName)override;
	virtual void stop(const QByteArray &programName)override;
	virtual void restart(const QByteArray &programName)override;
	virtual bool listConfigOptions(const QByteArray &programName,QList<IotServerGDILConfigOption> &options)override;
	virtual bool setConfigOption(const QByteArray &programName,const ConfigOptionId &id,const DataUnit &data)override;
	virtual bool listTimers(const QByteArray &programName,QList<IotServerGDILTimer> &timers)override;
	virtual bool setTimer(const QByteArray &programName,quint32 blockId,const TimerBlock::TimerConfig &cfg)override;
	bool reloadPrograms();

private slots:
	void onConnected();
	void onDisconnected();

private:
	IotServerConnection *srvConn;
	IotServerCommands *srvCmds;
	QMap<QByteArray,bool> programsMap;
	bool ready;
};

#endif // IOTSERVERGDILPROGRAMS_H
