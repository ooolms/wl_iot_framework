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
	GDILProgramsManager *mgr=ServerInstance::inst().gdilPrograms();
	if(ctx.cmd=="gdil_list")
	{
		QStringList progs=mgr->programs(proc->uid());
		for(const QString &s:progs)
		{
			ctx.retVal.append(s.toUtf8());
			ctx.retVal.append(mgr->isWorking(proc->uid(),s)?"1":"0");
		}
		return true;
	}
	if(ctx.args.count()<1)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QString script=QString::fromUtf8(ctx.args[0]);
	if(ctx.cmd=="gdil_start")
	{
		if(!mgr->startStopProgram(proc->uid(),script,true))
		{
			ctx.retVal.append("no program found: "+ctx.args[0]);
			return false;
		}
		return true;
	}
	else if(ctx.cmd=="gdil_stop")
	{
		if(!mgr->startStopProgram(proc->uid(),script,false))
		{
			ctx.retVal.append("no program found: "+ctx.args[0]);
			return false;
		}
		return true;
	}
	else if(ctx.cmd=="gdil_restart")
	{
		if(!mgr->startStopProgram(proc->uid(),script,false))
		{
			ctx.retVal.append("no program found: "+ctx.args[0]);
			return false;
		}
		if(!mgr->startStopProgram(proc->uid(),script,true))
		{
			ctx.retVal.append("no program found: "+ctx.args[0]);
			return false;
		}
		return true;
	}
	else if(ctx.cmd=="gdil_upload")
	{
		if(ctx.args.count()<2)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		QByteArray text=ctx.args[1];
		if(mgr->programs(proc->uid()).contains(script))
		{
			if(!mgr->updateProgram(proc->uid(),script,text))
			{
				ctx.retVal.append("error when updating program");
				return false;
			}
			return true;
		}
		else
		{
			if(!mgr->addProgram(proc->uid(),script,text))
			{
				ctx.retVal.append("error when adding new program");
				return false;
			}
			return true;
		}
	}
	else if(ctx.cmd=="gdil_remove")
	{
		if(!mgr->removeProgram(proc->uid(),script))
		{
			ctx.retVal.append("error when removing program");
			return false;
		}
		return true;
	}
	else if(ctx.cmd=="gdil_get")
	{
		QByteArray text;
		if(!mgr->getProgram(proc->uid(),script,text))
		{
			ctx.retVal.append("no program found: "+ctx.args[0]);
			return false;
		}
		ctx.retVal.append(text);
		return true;
	}
	return false;
}

QByteArrayList GDILControlCommand::acceptedCommands()
{
	return QByteArrayList()<<"gdil_start"<<"gdil_stop"<<"gdil_restart"<<
		"gdil_list"<<"gdil_upload"<<"gdil_remove"<<"gdil_get";
}
