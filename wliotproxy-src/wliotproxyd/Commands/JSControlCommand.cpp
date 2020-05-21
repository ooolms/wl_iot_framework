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

#include "JSControlCommand.h"
#include "../ServerInstance.h"
#include "StandardErrors.h"

JSControlCommand::JSControlCommand(CommandProcessor *p)
	:ICommand(p)
{
}

bool JSControlCommand::processCommand(CallContext &ctx)
{
	JSScriptsManager *mgr=ServerInstance::inst().jsScripts();
	if(ctx.cmd=="js_list")
	{
		QByteArrayList progs=mgr->programs(proc->uid());
		for(const QByteArray &s:progs)
		{
			ctx.retVal.append(s);
			ctx.retVal.append(mgr->isWorking(proc->uid(),s)?"1":"0");
		}
		return true;
	}
	if(ctx.args.count()<1)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray name=ctx.args[0];
	if(ctx.cmd=="js_start")
	{
		if(!mgr->startStopProgram(proc->uid(),name,true))
		{
			ctx.retVal.append("no script found: "+ctx.args[0]);
			return false;
		}
		return true;
	}
	else if(ctx.cmd=="js_stop")
	{
		if(!mgr->startStopProgram(proc->uid(),name,false))
		{
			ctx.retVal.append("no script found: "+ctx.args[0]);
			return false;
		}
		return true;
	}
	else if(ctx.cmd=="js_restart")
	{
		if(!mgr->startStopProgram(proc->uid(),name,false))
		{
			ctx.retVal.append("no script found: "+ctx.args[0]);
			return false;
		}
		if(!mgr->startStopProgram(proc->uid(),name,true))
		{
			ctx.retVal.append("no script found: "+ctx.args[0]);
			return false;
		}
		return true;
	}
	else if(ctx.cmd=="js_upload")
	{
		if(ctx.args.count()<2)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		QByteArray text=ctx.args[1];
		if(mgr->programs(proc->uid()).contains(name))
		{
			if(!mgr->updateProgram(proc->uid(),name,text))
			{
				ctx.retVal.append("error when updating script");
				return false;
			}
			return true;
		}
		else
		{
			if(!mgr->addProgram(proc->uid(),name,text))
			{
				ctx.retVal.append("error when adding new script");
				return false;
			}
			return true;
		}
	}
	else if(ctx.cmd=="js_remove")
	{
		if(!mgr->removeProgram(proc->uid(),name))
		{
			ctx.retVal.append("error when removing script");
			return false;
		}
		return true;
	}
	else if(ctx.cmd=="js_get")
	{
		QByteArray text;
		if(!mgr->getProgram(proc->uid(),name,text))
		{
			ctx.retVal.append("no script found: "+ctx.args[0]);
			return false;
		}
		ctx.retVal.append(text);
		return true;
	}
	return false;
}

QByteArrayList JSControlCommand::acceptedCommands()
{
	return QByteArrayList()<<"js_start"<<"js_stop"<<"js_restart"<<"js_list"<<"js_upload"<<"js_remove"<<"js_get";
}
