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

#include "IotServerGDILProgramsCommands.h"

IotServerGDILProgramsCommands::IotServerGDILProgramsCommands(IotServerConnection *conn)
	:QObject(conn)
{
	srvConn=conn;
}

bool IotServerGDILProgramsCommands::list(QByteArrayList &programs,QList<bool> &states)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("gdil_list",QByteArrayList(),retVal)||(retVal.count()%2)!=0)
		return false;
	programs.clear();
	states.clear();
	for(int i=0;i<retVal.count()>>1;++i)
	{
		programs.append(retVal[i<<1]);
		states.append(retVal[(i<<1)+1]=="1");
	}
	return true;
}

bool IotServerGDILProgramsCommands::get(const QByteArray &programName,QByteArray &text)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("gdil_get",QByteArrayList()<<programName,retVal)||retVal.count()<1)
		return false;
	text.append(retVal[0]);
	return true;
}

bool IotServerGDILProgramsCommands::remove(const QByteArray &programName)
{
	return srvConn->execCommand("gdil_remove",QByteArrayList()<<programName);
}

bool IotServerGDILProgramsCommands::upload(const QByteArray &programName,const QByteArray &text)
{
	return srvConn->execCommand("gdil_upload",QByteArrayList()<<programName<<text);
}

bool IotServerGDILProgramsCommands::start(const QByteArray &programName)
{
	return srvConn->execCommand("gdil_start",QByteArrayList()<<programName);
}

bool IotServerGDILProgramsCommands::stop(const QByteArray &programName)
{
	return srvConn->execCommand("gdil_stop",QByteArrayList()<<programName);
}

bool IotServerGDILProgramsCommands::restart(const QByteArray &programName)
{
	return srvConn->execCommand("gdil_restart",QByteArrayList()<<programName);
}

bool IotServerGDILProgramsCommands::listConfigOptions(const QByteArray &programName,
	QList<IotServerGDILConfigOption> &options)
{
	options.clear();
	CmDataCallback cb=[&options](const QByteArrayList &args)->bool
	{
		if(args.count()<8)
			return false;
		bool ok=false;
		quint32 blockId=args[0].toUInt(&ok);
		if(!ok)return false;
		QByteArray blockName=args[1];
		QByteArray key=args[2];
		ConfigOptionId id={blockId,key};
		QByteArrayList constrTypesStr=args[3].split(',');
		TypeConstraints constr;
		for(const QByteArray &b:constrTypesStr)
		{
			DataUnit::Type t=DataUnit::typeFromStr(b);
			if(t==DataUnit::INVALID)return false;
			constr.types|=t;
		}
		constr.dim=args[4].toUInt(&ok);
		if(!ok)return false;
		DataUnit::Type t=DataUnit::typeFromStr(args[5]);
		if(t==DataUnit::INVALID)return false;
		quint32 dim=args[6].toUInt(&ok);
		if(!ok)return false;
		DataUnit u(t,dim,args.mid(7));
		if(!u.isValid())return false;
		options.append(IotServerGDILConfigOption(id,blockName,u,constr));
		return true;
	};
	return srvConn->execCommand("gdil_list_config_options",QByteArrayList()<<programName,cb);
}

bool IotServerGDILProgramsCommands::setConfigOption(const QByteArray &programName,
	const ConfigOptionId &id,const DataUnit &data)
{
	return srvConn->execCommand("gdil_set_config_option",QByteArrayList()<<programName<<QByteArray::number(id.blockId)<<
	id.key<<DataUnit::typeToStr(data.type())<<QByteArray::number(data.dim())<<data.value()->dumpToMsgArgs());
}

bool IotServerGDILProgramsCommands::listTimers(const QByteArray &programName,QList<IotServerGDILTimer> &timers)
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
		TimerBlock::TimerConfig cfg={QDateTime::fromSecsSinceEpoch(time),pol,repeatInterval};
		timers.append(IotServerGDILTimer(blockId,name,cfg));
		return true;
	};
	return srvConn->execCommand("gdil_list_timers",QByteArrayList()<<programName,cb);
}

bool IotServerGDILProgramsCommands::setTimer(const QByteArray &programName,
	quint32 blockId,const TimerBlock::TimerConfig &cfg)
{
	return srvConn->execCommand("gdil_set_timer",QByteArrayList()<<programName<<QByteArray::number(blockId)<<
		QByteArray::number(cfg.startTime.toSecsSinceEpoch())<<TimerBlock::policyToStr(cfg.policy)<<
		QByteArray::number(cfg.repeatInterval));
}
