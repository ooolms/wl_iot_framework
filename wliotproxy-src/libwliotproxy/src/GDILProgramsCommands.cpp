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

#include "wliot/client/GDILProgramsCommands.h"

using namespace WLIOT;
using namespace WLIOTClient;
using namespace WLIOTGDIL;

GDILProgramsCommands::GDILProgramsCommands(ServerConnection *conn)
	:BaseProgramsControlCommands("gdil",conn)
{
	srvConn=conn;
}

bool GDILProgramsCommands::listConfigOptions(const QByteArray &id,
	QList<GDILConfigOption> &options)
{
	options.clear();
	CmDataCallback cb=[&options](const QByteArrayList &args)->bool
	{
		if(args.count()<6)
			return false;
		bool ok=false;
		quint32 blockId=args[0].toUInt(&ok);
		if(!ok)return false;
		QByteArray blockName=args[1];
		QByteArray key=args[2];
		ConfigOptionId id={blockId,key};
		if(!ok)return false;
		DataUnit::Type t=DataUnit::typeFromStr(args[3]);
		if(t==DataUnit::INVALID)return false;
		quint32 dim=args[4].toUInt(&ok);
		if(!ok)return false;
		DataUnit u(t,dim,args.mid(5));
		if(!u.isValid())return false;
		options.append(GDILConfigOption(id,blockName,u));
		return true;
	};
	return srvConn->execCommand("gdil_list_config_options",QByteArrayList()<<id,cb);
}

bool GDILProgramsCommands::setConfigOption(const QByteArray &id,const ConfigOptionId &optionId,const DataUnit &data)
{
	return srvConn->execCommand("gdil_set_config_option",QByteArrayList()<<id<<QByteArray::number(optionId.blockId)<<
	optionId.key<<DataUnit::typeToStr(data.type())<<QByteArray::number(data.dim())<<data.value()->dumpToMsgArgs());
}

bool GDILProgramsCommands::listTimers(const QByteArray &id,QList<GDILTimer> &timers)
{
	timers.clear();
	CmDataCallback cb=[&timers](const QByteArrayList &args)->bool
	{
		if(args.count()!=5)return false;
		bool ok=false;
		quint32 blockId=args[0].toUInt(&ok);
		if(!ok)return false;
		QByteArray name=args[1];
		if(name.isEmpty())name="<untitled>";
		qint64 time=args[2].toLongLong(&ok);
		if(!ok)return false;
		TimerBlock::SchedulePolicy pol=TimerBlock::policyFromStr(args[3]);
		qint64 repeatInterval=args[4].toLongLong(&ok);
		if(!ok||pol==TimerBlock::INVALID)return false;
		TimerBlock::TimerConfig cfg={QDateTime::fromMSecsSinceEpoch(time*1000),pol,repeatInterval};
		timers.append(GDILTimer(blockId,name,cfg));
		return true;
	};
	return srvConn->execCommand("gdil_list_timers",QByteArrayList()<<id,cb);
}

bool GDILProgramsCommands::setTimer(const QByteArray &id,quint32 blockId,const TimerBlock::TimerConfig &cfg)
{
	return srvConn->execCommand("gdil_set_timer",QByteArrayList()<<id<<QByteArray::number(blockId)<<
		QByteArray::number(cfg.startTime.toMSecsSinceEpoch()/1000)<<TimerBlock::policyToStr(cfg.policy)<<
		QByteArray::number(cfg.repeatInterval));
}
