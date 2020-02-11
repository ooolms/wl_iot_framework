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
		QStringList progs=mgr->scripts(proc->uid());
		for(const QString &s:progs)
			ctx.retVal.append(s.toUtf8());
		return true;
	}
	if(ctx.args.count()<1)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QString script=QString::fromUtf8(ctx.args[0]);
	if(ctx.cmd=="js_start")
	{
		if(!mgr->startStopScript(proc->uid(),script,true))
		{
			ctx.retVal.append("no script found: "+ctx.args[0]);
			return false;
		}
		return true;
	}
	else if(ctx.cmd=="js_stop")
	{
		if(!mgr->startStopScript(proc->uid(),script,false))
		{
			ctx.retVal.append("no script found: "+ctx.args[0]);
			return false;
		}
		return true;
	}
	else if(ctx.cmd=="js_restart")
	{
		if(!mgr->startStopScript(proc->uid(),script,false))
		{
			ctx.retVal.append("no script found: "+ctx.args[0]);
			return false;
		}
		if(!mgr->startStopScript(proc->uid(),script,true))
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
		if(mgr->scripts(proc->uid()).contains(script))
		{
			if(!mgr->updateScript(proc->uid(),script,text))
			{
				ctx.retVal.append("error when updating script");
				return false;
			}
			return true;
		}
		else
		{
			if(!mgr->addScript(proc->uid(),script,text))
			{
				ctx.retVal.append("error when adding new script");
				return false;
			}
			return true;
		}
	}
	else if(ctx.cmd=="js_remove")
	{
		if(!mgr->removeScript(proc->uid(),script))
		{
			ctx.retVal.append("error when removing script");
			return false;
		}
		return true;
	}
	else if(ctx.cmd=="js_get")
	{
		QByteArray text;
		if(!mgr->getScript(proc->uid(),script,text))
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
