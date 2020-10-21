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

#include "wliot/client/VDILProgramsCommands.h"

using namespace WLIOT;
using namespace WLIOTClient;
using namespace WLIOTVDIL;

VDILProgramsCommands::VDILProgramsCommands(ServerConnection *conn)
	:BaseProgramsControlCommands("vdil",conn)
{
	srvConn=conn;
}

bool VDILProgramsCommands::listConfigOptions(const QByteArray &id,
	QList<VDILConfigOption> &options)
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
		DataUnit u(TypeAndDim(t,dim),args.mid(5));
		if(!u.isValid())return false;
		options.append(VDILConfigOption(id,blockName,u));
		return true;
	};
	return srvConn->execCommand("vdil_list_config_options",QByteArrayList()<<id,cb);
}

bool VDILProgramsCommands::setConfigOption(const QByteArray &id,const ConfigOptionId &optionId,const DataUnit &data)
{
	return srvConn->execCommand("vdil_set_config_option",QByteArrayList()<<id<<QByteArray::number(optionId.blockId)<<
	optionId.key<<DataUnit::typeToStr(data.type())<<QByteArray::number(data.dim())<<data.value()->dumpToMsgArgs());
}

bool VDILProgramsCommands::listTimers(const QByteArray &id,QList<VDILTimer> &timers)
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
		timers.append(VDILTimer(blockId,name,cfg));
		return true;
	};
	return srvConn->execCommand("vdil_list_timers",QByteArrayList()<<id,cb);
}

bool VDILProgramsCommands::setTimer(const QByteArray &id,quint32 blockId,const TimerBlock::TimerConfig &cfg)
{
	return srvConn->execCommand("vdil_set_timer",QByteArrayList()<<id<<QByteArray::number(blockId)<<
		QByteArray::number(cfg.startTime.toMSecsSinceEpoch()/1000)<<TimerBlock::policyToStr(cfg.policy)<<
		QByteArray::number(cfg.repeatInterval));
}
