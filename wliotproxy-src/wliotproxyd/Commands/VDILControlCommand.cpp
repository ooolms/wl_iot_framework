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

#include "VDILControlCommand.h"
#include "../ServerInstance.h"
#include "../VDILDataProcessing/VDILEngine.h"
#include "../VDILDataProcessing/VDILProgramConfigDb.h"
#include "StandardErrors.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

VDILControlCommand::VDILControlCommand(CommandProcessor *p)
	:BaseProgramsControlCommand("vdil",ServerInstance::inst().vdilPrograms(),p)
{
}

bool VDILControlCommand::processCommand(ICommand::CallContext &ctx)
{
	if(ctx.cmd=="vdil_list_config_options")
	{
		if(ctx.args.count()!=1)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments());
			return false;
		}
		QByteArray programId=ctx.args[0];
		VDILEngine *e=(VDILEngine*)ServerInstance::inst().vdilPrograms()->engine(proc->uid(),programId);
		if(!e)
		{
			ctx.retVal.append(noProgramFoundError(ctx.args[0]));
			return false;
		}
		Program *p=e->program();
		if(!p)
		{
			ctx.retVal.append("program data was not parsed due to errors");
			return false;
		}
		QList<ConfigOptionId> ids=p->allConfigOptions();
		for(ConfigOptionId &id:ids)
		{
			BaseBlock *b=p->selfBlocks().value(id.blockId);
			if(!b)continue;
			DataUnit v=p->configOptionValue(id);
			writeCmdataMsg(ctx.callId,QByteArrayList()<<QByteArray::number(id.blockId)<<b->title.toUtf8()<<id.key<<
				DataUnit::typeToStr(v.type())<<QByteArray::number(v.dim())<<v.value()->dumpToMsgArgs());
		}
		return true;
	}
	else if(ctx.cmd=="vdil_set_config_option")
	{
		if(ctx.args.count()<6)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments());
			return false;
		}
		QByteArray programId=ctx.args[0];
		VDILEngine *e=(VDILEngine*)ServerInstance::inst().vdilPrograms()->engine(proc->uid(),programId);
		VDILProgramConfigDb *cfgDb=(VDILProgramConfigDb*)
			ServerInstance::inst().vdilPrograms()->cfgDb(proc->uid(),programId);
		if(!e||!cfgDb)
		{
			ctx.retVal.append(noProgramFoundError(ctx.args[0]));
			return false;
		}
		ConfigOptionId id;
		bool ok1=false,ok2=false;
		id.blockId=ctx.args[1].toUInt(&ok1);
		id.key=ctx.args[2];
		DataUnit::Type t=DataUnit::typeFromStr(ctx.args[3]);
		quint32 dim=ctx.args[4].toUInt(&ok2);
		if(!ok1||!ok2||id.key.isEmpty()||t==DataUnit::INVALID)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments());
			return false;
		}
		DataUnit u(TypeAndDim(t,dim),ctx.args.mid(5));
		if(!u.isValid())
		{
			ctx.retVal.append(StandardErrors::invalidAgruments());
			return false;
		}
		if(!e->program()||!e->program()->setConfigOptionValue(id,u))
		{
			ctx.retVal.append("config value was not set");
			return false;
		}
		cfgDb->setConfigOption(id,u);
		return true;
	}
	else if(ctx.cmd=="vdil_list_timers")
	{
		if(ctx.args.count()!=1)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments());
			return false;
		}
		QByteArray programId=ctx.args[0];
		VDILEngine *e=(VDILEngine*)ServerInstance::inst().vdilPrograms()->engine(proc->uid(),programId);
		if(!e)
		{
			ctx.retVal.append(noProgramFoundError(ctx.args[0]));
			return false;
		}
		if(!e->program())
		{
			ctx.retVal.append("program data was not parsed due to errors");
			return false;
		}
		const QMap<quint32,TimerBlock*> &timers=e->program()->timerBlocks();
		for(TimerBlock *t:timers)
		{
			if(!t->configurable())continue;
			TimerBlock::TimerConfig cfg=t->config();
			writeCmdataMsg(ctx.callId,QByteArrayList()<<QByteArray::number(t->blockId())<<t->title.toUtf8()<<
				QByteArray::number(cfg.startTime.toMSecsSinceEpoch()/1000)<<TimerBlock::policyToStr(cfg.policy)<<
				QByteArray::number(cfg.repeatInterval));
		}
		return true;
	}
	else if(ctx.cmd=="vdil_set_timer")
	{
		if(ctx.args.count()!=5)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments());
			return false;
		}
		QByteArray programId=ctx.args[0];
		VDILEngine *e=(VDILEngine*)ServerInstance::inst().vdilPrograms()->engine(proc->uid(),programId);
		VDILProgramConfigDb *cfgDb=(VDILProgramConfigDb*)
			ServerInstance::inst().vdilPrograms()->cfgDb(proc->uid(),programId);
		if(!e||!cfgDb)
		{
			ctx.retVal.append(noProgramFoundError(ctx.args[0]));
			return false;
		}
		if(!e->program())
		{
			ctx.retVal.append("program data was not parsed due to errors");
			return false;
		}
		bool ok1=false,ok2=false,ok3=false;
		quint32 blockId=ctx.args[1].toUInt(&ok1);
		TimerBlock *b=e->program()->timerBlocks().value(blockId);
		TimerBlock::TimerConfig cfg;
		cfg.startTime=QDateTime::fromMSecsSinceEpoch(ctx.args[2].toLongLong(&ok2)*1000);
		cfg.policy=TimerBlock::policyFromStr(ctx.args[3]);
		cfg.repeatInterval=ctx.args[4].toLongLong(&ok3);
		if(!b||!ok1||!ok2||!ok3||cfg.policy==TimerBlock::INVALID||!b->configurable())
		{
			ctx.retVal.append(StandardErrors::invalidAgruments());
			return false;
		}
		b->setConfig(cfg,true);
		cfgDb->setTimerConfig(blockId,cfg);
		e->stop();
		e->start();
		return true;
	}
	else return BaseProgramsControlCommand::processCommand(ctx);
}

QByteArrayList VDILControlCommand::acceptedCommands()
{
	return BaseProgramsControlCommand::acceptedCommands()<<
		"vdil_list_config_options"<<"vdil_set_config_option"<<"vdil_list_timers"<<"vdil_set_timer";
}
