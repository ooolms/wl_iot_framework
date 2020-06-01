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
#include "../GDILDataProcessing/GDILEngine.h"
#include "StandardErrors.h"

using namespace WLIOT;
using namespace WLIOTGDIL;

GDILControlCommand::GDILControlCommand(CommandProcessor *p)
	:BaseProgramsControlCommand("gdil",ServerInstance::inst().gdilPrograms(),p)
{
}

bool GDILControlCommand::processCommand(ICommand::CallContext &ctx)
{
	if(ctx.cmd=="gdil_list_config_options")
	{
		if(ctx.args.count()!=1)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments());
			return false;
		}
		QByteArray programId=ctx.args[0];
		GDILEngine *e=(GDILEngine*)ServerInstance::inst().gdilPrograms()->engine(proc->uid(),programId);
		if(!e)
		{
			ctx.retVal.append(noProgramFoundError(ctx.args[0]));
			return false;
		}
		QList<ConfigOptionId> ids=e->program()->allConfigOptions();
		for(ConfigOptionId &id:ids)
		{
			BaseBlock *b=e->program()->blockById(id.blockId);
			if(!b)continue;
			DataUnit v=e->program()->configOptionValue(id);
			writeCmdataMsg(ctx.callId,QByteArrayList()<<QByteArray::number(id.blockId)<<b->title.toUtf8()<<id.key<<
				DataUnit::typeToStr(v.type())<<QByteArray::number(v.dim())<<v.value()->dumpToMsgArgs());
		}
		return true;
	}
	else if(ctx.cmd=="gdil_set_config_option")
	{
		if(ctx.args.count()<6)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments());
			return false;
		}
		QByteArray programId=ctx.args[0];
		GDILEngine *e=(GDILEngine*)ServerInstance::inst().gdilPrograms()->engine(proc->uid(),programId);
		if(!e)
		{
			ctx.retVal.append(noProgramFoundError(ctx.args[0]));
			return false;
		}
		ConfigOptionId id;
		bool ok1=false,ok2=false;;
		id.blockId=ctx.args[1].toUInt(&ok1);
		id.key=ctx.args[2];
		DataUnit::Type t=DataUnit::typeFromStr(ctx.args[3]);
		quint32 dim=ctx.args[4].toUInt(&ok2);
		if(!ok1||!ok2||id.key.isEmpty()||t==DataUnit::INVALID)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments());
			return false;
		}
		DataUnit u(t,dim,ctx.args.mid(5));
		if(!u.isValid())
		{
			ctx.retVal.append(StandardErrors::invalidAgruments());
			return false;
		}
		if(!e->program()->setConfigOptionValue(id,u))
		{
			ctx.retVal.append("config value was not set");
			return false;
		}
		return true;
	}
	else if(ctx.cmd=="gdil_list_timers")
	{
		if(ctx.args.count()!=1)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments());
			return false;
		}
		QByteArray programId=ctx.args[0];
		GDILEngine *e=(GDILEngine*)ServerInstance::inst().gdilPrograms()->engine(proc->uid(),programId);
		if(!e)
		{
			ctx.retVal.append(noProgramFoundError(ctx.args[0]));
			return false;
		}
		const QMap<quint32,TimerBlock*> &timers=e->program()->timerBlocks();
		for(TimerBlock *t:timers)
		{
			TimerBlock::TimerConfig cfg=t->config();
			writeCmdataMsg(ctx.callId,QByteArrayList()<<QByteArray::number(t->blockId())<<t->title.toUtf8()<<
				QByteArray::number(cfg.startTime.toMSecsSinceEpoch()/1000)<<TimerBlock::policyToStr(cfg.policy)<<
				QByteArray::number(cfg.repeatInterval));
		}
		return true;
	}
	else if(ctx.cmd=="gdil_set_timer")
	{
		if(ctx.args.count()!=5)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments());
			return false;
		}
		QByteArray programId=ctx.args[0];
		GDILEngine *e=(GDILEngine*)ServerInstance::inst().gdilPrograms()->engine(proc->uid(),programId);
		if(!e)
		{
			ctx.retVal.append(noProgramFoundError(ctx.args[0]));
			return false;
		}
		bool ok1=false,ok2=false,ok3=false;
		quint32 blockId=ctx.args[1].toUInt(&ok1);
		TimerBlock *b=e->program()->timerBlocks().value(blockId);
		TimerBlock::TimerConfig cfg;
		cfg.startTime=QDateTime::fromMSecsSinceEpoch(ctx.args[2].toLongLong(&ok2)*1000);
		cfg.policy=TimerBlock::policyFromStr(ctx.args[3]);
		cfg.repeatInterval=ctx.args[4].toLongLong(&ok3);
		if(!b||!ok1||!ok2||!ok3||cfg.policy==TimerBlock::INVALID)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments());
			return false;
		}
		b->setConfig(cfg,true);
		return true;
	}
	else return BaseProgramsControlCommand::processCommand(ctx);
}

QByteArrayList GDILControlCommand::acceptedCommands()
{
	return BaseProgramsControlCommand::acceptedCommands()<<
		"gdil_list_config_options"<<"gdil_set_config_option"<<"gdil_list_timers"<<"gdil_set_timer";
}
