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

bool IotServerGDILProgramsCommands::listConfigOptions(
	const QByteArray &programName,QMap<ConfigOptionId,TypeConstraints> &configOptionConstraints,
	QMap<ConfigOptionId,DataUnit> &configOptionValues)
{
	configOptionConstraints.clear();
	configOptionValues.clear();
	CmDataCallback cb=[&configOptionConstraints,&configOptionValues](const QByteArrayList &args)->bool
	{
		if(args.count()!=7)
			return false;
		bool ok=false;
		quint32 blockId=args[0].toUInt(&ok);
		if(!ok)return false;
		QByteArray key=args[1];
		ConfigOptionId id={blockId,key};
		QByteArrayList constr=args[2].split(',');
		TypeConstraints c;
		for(const QByteArray &b:constr)
		{
			DataUnit::Type t=DataUnit::typeFromStr(b);
			if(t==DataUnit::INVALID)return false;
			c.types&=t;
		}
		c.dim=args[3].toUInt(&ok);
		if(!ok)return false;
		DataUnit::Type t=DataUnit::typeFromStr(args[4]);
		if(t==DataUnit::INVALID)return false;
		quint32 dim=args[5].toUInt(&ok);
		if(!ok)return false;
		DataUnit u=DataUnit::valueFromMsgArgs(t,dim,args.mid(6));
		if(!u.isValid())return false;
		configOptionConstraints[id]=c;
		configOptionValues[id]=u;
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

bool IotServerGDILProgramsCommands::listTimers(const QByteArray &programName,
	QMap<quint32,TimerBlock::TimerConfig> &timers,QMap<quint32,QByteArray> &timerNames)
{
	timers.clear();
	timerNames.clear();
	CmDataCallback cb=[&timers,&timerNames](const QByteArrayList &args)->bool
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
		timers[blockId]=cfg;
		timerNames[blockId]=name;
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
