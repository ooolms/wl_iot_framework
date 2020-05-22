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

#include "GDILControlCommand.h"
#include "../ServerInstance.h"
#include "StandardErrors.h"
#include <GDIL/core/Program.h>
#include "../GDILDataProcessing/GDILEngine.h"
#include "../GDILDataProcessing/GDILProgramConfigDb.h"

using namespace WLIOT;
using namespace WLIOTGDIL;

GDILControlCommand::GDILControlCommand(CommandProcessor *p)
	:ICommand(p)
{
}

bool GDILControlCommand::processCommand(CallContext &ctx)
{
	mgr=ServerInstance::inst().gdilPrograms();
	if(ctx.cmd=="gdil_list")
		return list(ctx);
	if(ctx.args.count()<1)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray name=ctx.args.takeAt(0);
	if(ctx.cmd=="gdil_start")
		return start(name,ctx);
	else if(ctx.cmd=="gdil_stop")
		return stop(name,ctx);
	else if(ctx.cmd=="gdil_restart")
		return restart(name,ctx);
	else if(ctx.cmd=="gdil_upload")
		return upload(name,ctx);
	else if(ctx.cmd=="gdil_remove")
		return remove(name,ctx);
	else if(ctx.cmd=="gdil_get")
		return get(name,ctx);
	else if(ctx.cmd=="gdil_list_config_options")
		return listConfigOptions(name,ctx);
	else if(ctx.cmd=="gdil_set_config_option")
		return setConfigOption(name,ctx);
	else if(ctx.cmd=="gdil_list_timers")
		return listTimers(name,ctx);
	else if(ctx.cmd=="gdil_set_timer")
		return setTimer(name,ctx);
	return false;
}

QByteArrayList GDILControlCommand::acceptedCommands()
{
	return QByteArrayList()<<"gdil_start"<<"gdil_stop"<<"gdil_restart"<<
		"gdil_list"<<"gdil_upload"<<"gdil_remove"<<"gdil_get"<<"gdil_list_config_options"<<
		"gdil_set_config_option"<<"gdil_list_timers"<<"gdil_set_timer";
}

bool GDILControlCommand::list(ICommand::CallContext &ctx)
{
	QByteArrayList progs=mgr->programs(proc->uid());
	for(const QByteArray &s:progs)
	{
		ctx.retVal.append(s);
		ctx.retVal.append(mgr->isWorking(proc->uid(),s)?"1":"0");
	}
	return true;
}

bool GDILControlCommand::start(const QByteArray &name,ICommand::CallContext &ctx)
{
	if(!mgr->startStopProgram(proc->uid(),name,true))
	{
		ctx.retVal.append("no program found: "+name);
		return false;
	}
	return true;
}

bool GDILControlCommand::stop(const QByteArray &name,ICommand::CallContext &ctx)
{
	if(!mgr->startStopProgram(proc->uid(),name,false))
	{
		ctx.retVal.append("no program found: "+name);
		return false;
	}
	return true;
}

bool GDILControlCommand::restart(const QByteArray &name,ICommand::CallContext &ctx)
{
	if(!mgr->startStopProgram(proc->uid(),name,false))
	{
		ctx.retVal.append("no program found: "+name);
		return false;
	}
	if(!mgr->startStopProgram(proc->uid(),name,true))
	{
		ctx.retVal.append("no program found: "+name);
		return false;
	}
	return true;
}

bool GDILControlCommand::get(const QByteArray &name,ICommand::CallContext &ctx)
{
	QByteArray text;
	if(!mgr->getProgram(proc->uid(),name,text))
	{
		ctx.retVal.append("no program found: "+name);
		return false;
	}
	ctx.retVal.append(text);
	return true;
}

bool GDILControlCommand::upload(const QByteArray &name,ICommand::CallContext &ctx)
{
	if(ctx.args.count()<1)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray text=ctx.args[0];
	if(mgr->programs(proc->uid()).contains(name))
	{
		if(!mgr->updateProgram(proc->uid(),name,text))
		{
			ctx.retVal.append("error when updating program");
			return false;
		}

		return true;
	}
	else
	{
		if(!mgr->addProgram(proc->uid(),name,text))
		{
			ctx.retVal.append("error when adding new program");
			return false;
		}
		return true;
	}
}

bool GDILControlCommand::remove(const QByteArray &name,ICommand::CallContext &ctx)
{
	if(!mgr->removeProgram(proc->uid(),name))
	{
		ctx.retVal.append("error when removing program");
		return false;
	}
	return true;
}

bool GDILControlCommand::listConfigOptions(const QByteArray &name,CallContext &ctx)
{
	Program *p=((GDILEngine*)mgr->engine(proc->uid(),name))->program();
	if(!p)
	{
		ctx.retVal.append("no program found: "+name);
		return false;
	}
	QList<ConfigOptionId> ids=p->allConfigOptions();
	for(ConfigOptionId &id:ids)
	{
		TypeConstraints c=p->configOptionConstraints(id);
		DataUnit u=p->configOptionValue(id);
		QByteArrayList typesList;
		if(c.types&DataUnit::BOOL)
			typesList.append(DataUnit::typeToStr(DataUnit::BOOL));
		if(c.types&DataUnit::SINGLE)
			typesList.append(DataUnit::typeToStr(DataUnit::SINGLE));
		if(c.types&DataUnit::ARRAY)
			typesList.append(DataUnit::typeToStr(DataUnit::ARRAY));
		QByteArray type;
		writeCmdataMsg(ctx.callId,QByteArrayList()<<QByteArray::number(id.blockId)<<
			p->blockById(id.blockId)->title.toUtf8()<<id.key<<
			typesList.join(',')<<QByteArray::number(c.dim)<<DataUnit::typeToStr(u.type())<<
			QByteArray::number(u.dim())<<u.value()->dumpToMsgArgs());
	}
	return true;
}

bool GDILControlCommand::setConfigOption(const QByteArray &name,ICommand::CallContext &ctx)
{
	if(ctx.args.count()<5)
		return false;
	bool ok=false;
	quint32 blockId=ctx.args[0].toUInt(&ok);
	if(!ok)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QString scriptU=QString::fromUtf8(name);
	QByteArray key=ctx.args[1];
	DataUnit::Type t=DataUnit::typeFromStr(ctx.args[2]);
	quint32 dim=ctx.args[3].toUInt(&ok);
	if(!ok||key.isEmpty()||t==DataUnit::INVALID)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	DataUnit val=DataUnit(t,dim,ctx.args.mid(4));
	if(!val.isValid())
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	GDILEngine *e=(GDILEngine*)mgr->engine(proc->uid(),name);
	GDILProgramConfigDb *cfgDb=(GDILProgramConfigDb*)mgr->cfgDb(proc->uid(),name);
	if(!e||!cfgDb)
	{
		ctx.retVal.append("no program found: "+name);
		return false;
	}
	bool isRunning=e->isRunning();
	if(isRunning)
		e->stop();
	bool r=e->program()->setConfigOptionValue({blockId,key},val);
	if(r)cfgDb->setConfigOption({blockId,key},val);
	if(isRunning)
		e->start();
	return r;
}

bool GDILControlCommand::listTimers(const QByteArray &name,ICommand::CallContext &ctx)
{
	Program *p=((GDILEngine*)mgr->engine(proc->uid(),name))->program();
	if(!p)
	{
		ctx.retVal.append("no program found: "+name);
		return false;
	}
	const QMap<quint32,TimerBlock*> &timers=p->timerBlocks();
	for(auto i=timers.constBegin();i!=timers.constEnd();++i)
	{
		TimerBlock::TimerConfig cfg=i.value()->config();
		writeCmdataMsg(ctx.callId,QByteArrayList()<<QByteArray::number(i.key())<<i.value()->title.toUtf8()<<
			QByteArray::number(cfg.startTime.toSecsSinceEpoch())<<TimerBlock::policyToStr(cfg.policy)<<
			QByteArray::number(cfg.repeatInterval));
	}
	return true;
}

bool GDILControlCommand::setTimer(const QByteArray &name,ICommand::CallContext &ctx)
{
	Program *p=((GDILEngine*)mgr->engine(proc->uid(),name))->program();
	if(!p)
	{
		ctx.retVal.append("no program found: "+name);
		return false;
	}
	bool ok=false;
	quint32 blockId=ctx.args[0].toUInt(&ok);
	if(!ok)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	qint64 time=ctx.args[1].toLongLong(&ok);
	if(!ok)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	TimerBlock::SchedulePolicy pol=TimerBlock::policyFromStr(ctx.args[2]);
	qint64 repeatInterval=ctx.args[3].toLongLong(&ok);
	if(!ok||pol==TimerBlock::INVALID)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	TimerBlock::TimerConfig cfg={QDateTime::fromSecsSinceEpoch(time),pol,repeatInterval};
	const QMap<quint32,TimerBlock*> &timers=p->timerBlocks();
	if(!timers.contains(blockId))
	{
		ctx.retVal.append("no timer found");
		return false;
	}
	GDILEngine *e=(GDILEngine*)mgr->engine(proc->uid(),name);
	GDILProgramConfigDb *cfgDb=(GDILProgramConfigDb*)mgr->cfgDb(proc->uid(),name);
	if(!e||!cfgDb)
	{
		ctx.retVal.append("no program found: "+name);
		return false;
	}
	bool isRunning=e->isRunning();
	if(isRunning)
		e->stop();
	timers[blockId]->setConfig(cfg,true);
	cfgDb->setTimerConfig(blockId,cfg);
	if(isRunning)
		e->start();
	return true;
}
