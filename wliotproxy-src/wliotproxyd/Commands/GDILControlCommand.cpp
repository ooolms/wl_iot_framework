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
	QByteArray script=ctx.args.takeAt(0);
	if(ctx.cmd=="gdil_start")
		return start(script,ctx);
	else if(ctx.cmd=="gdil_stop")
		return stop(script,ctx);
	else if(ctx.cmd=="gdil_restart")
		return restart(script,ctx);
	else if(ctx.cmd=="gdil_upload")
		return upload(script,ctx);
	else if(ctx.cmd=="gdil_remove")
		return remove(script,ctx);
	else if(ctx.cmd=="gdil_get")
		return get(script,ctx);
	else if(ctx.cmd=="gdil_list_config_options")
		return listConfigOptions(script,ctx);
	else if(ctx.cmd=="gdil_set_config_option")
		return setConfigOption(script,ctx);
	else if(ctx.cmd=="gdil_list_timers")
		return listTimers(script,ctx);
	else if(ctx.cmd=="gdil_set_timer")
		return setTimer(script,ctx);
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
	QStringList progs=mgr->programs(proc->uid());
	for(const QString &s:progs)
	{
		ctx.retVal.append(s.toUtf8());
		ctx.retVal.append(mgr->isWorking(proc->uid(),s)?"1":"0");
	}
	return true;
}

bool GDILControlCommand::start(const QByteArray &script,ICommand::CallContext &ctx)
{
	if(!mgr->startStopProgram(proc->uid(),QString::fromUtf8(script),true))
	{
		ctx.retVal.append("no program found: "+script);
		return false;
	}
	return true;
}

bool GDILControlCommand::stop(const QByteArray &script,ICommand::CallContext &ctx)
{
	if(!mgr->startStopProgram(proc->uid(),QString::fromUtf8(script),false))
	{
		ctx.retVal.append("no program found: "+script);
		return false;
	}
	return true;
}

bool GDILControlCommand::restart(const QByteArray &script,ICommand::CallContext &ctx)
{
	QString scriptU=QString::fromUtf8(script);
	if(!mgr->startStopProgram(proc->uid(),scriptU,false))
	{
		ctx.retVal.append("no program found: "+script);
		return false;
	}
	if(!mgr->startStopProgram(proc->uid(),scriptU,true))
	{
		ctx.retVal.append("no program found: "+script);
		return false;
	}
	return true;
}

bool GDILControlCommand::get(const QByteArray &script,ICommand::CallContext &ctx)
{
	QByteArray text;
	if(!mgr->getProgram(proc->uid(),QString::fromUtf8(script),text))
	{
		ctx.retVal.append("no program found: "+script);
		return false;
	}
	ctx.retVal.append(text);
	return true;
}

bool GDILControlCommand::upload(const QByteArray &script,ICommand::CallContext &ctx)
{
	if(ctx.args.count()<1)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray text=ctx.args[0];
	QString scriptU=QString::fromUtf8(script);
	if(mgr->programs(proc->uid()).contains(scriptU))
	{
		if(!mgr->updateProgram(proc->uid(),scriptU,text))
		{
			ctx.retVal.append("error when updating program");
			return false;
		}
		return true;
	}
	else
	{
		if(!mgr->addProgram(proc->uid(),scriptU,text))
		{
			ctx.retVal.append("error when adding new program");
			return false;
		}
		return true;
	}
}

bool GDILControlCommand::remove(const QByteArray &script,ICommand::CallContext &ctx)
{
	if(!mgr->removeProgram(proc->uid(),QString::fromUtf8(script)))
	{
		ctx.retVal.append("error when removing program");
		return false;
	}
	return true;
}

bool GDILControlCommand::listConfigOptions(const QByteArray &script,CallContext &ctx)
{
	Program *p=mgr->program(proc->uid(),QString::fromUtf8(script));
	if(!p)
	{
		ctx.retVal.append("no program found: "+script);
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
		writeCmdataMsg(ctx.callId,QByteArrayList()<<QByteArray::number(id.blockId)<<id.key<<
			typesList.join(',')<<QByteArray::number(c.dim)<<DataUnit::typeToStr(u.type())<<
			QByteArray::number(u.dim())<<u.value()->dumpToMsgArgs());
	}
	return true;
}

bool GDILControlCommand::setConfigOption(const QByteArray &script,ICommand::CallContext &ctx)
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
	Program *p=mgr->program(proc->uid(),QString::fromUtf8(script));
	if(!p)
	{
		ctx.retVal.append("no program found: "+script);
		return false;
	}
	return p->setConfigOptionValue({blockId,key},val);
}

bool GDILControlCommand::listTimers(const QByteArray &script, ICommand::CallContext &ctx)
{
	Program *p=mgr->program(proc->uid(),QString::fromUtf8(script));
	if(!p)
	{
		ctx.retVal.append("no program found: "+script);
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

bool GDILControlCommand::setTimer(const QByteArray &script,ICommand::CallContext &ctx)
{
	Program *p=mgr->program(proc->uid(),QString::fromUtf8(script));
	if(!p)
	{
		ctx.retVal.append("no program found: "+script);
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
	timers[blockId]->setConfig(cfg,true);
	return true;
}
