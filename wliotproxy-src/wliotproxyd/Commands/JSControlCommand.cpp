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
#include "../IotProxyInstance.h"
#include "StandardErrors.h"

JSControlCommand::JSControlCommand(QtIODeviceWrap *d,IotProxyCommandProcessor *p)
	:ICommand(d,p)
{
}

bool JSControlCommand::processCommand(CallContext &ctx)
{
	if(ctx.cmd=="js_list")
	{
		QStringList progs=IotProxyInstance::inst().jsPrograms();
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
		if(!IotProxyInstance::inst().controlJSProgram(script,true))
		{
			ctx.retVal.append("no script found: "+ctx.args[0]);
			return false;
		}
		return true;
	}
	else if(ctx.cmd=="js_stop")
	{
		if(!IotProxyInstance::inst().controlJSProgram(script,false))
		{
			ctx.retVal.append("no script found: "+ctx.args[0]);
			return false;
		}
		return true;
	}
	else if(ctx.cmd=="js_restart")
	{
		if(!IotProxyInstance::inst().controlJSProgram(script,false))
		{
			ctx.retVal.append("no script found: "+ctx.args[0]);
			return false;
		}
		if(!IotProxyInstance::inst().controlJSProgram(script,true))
		{
			ctx.retVal.append("no script found: "+ctx.args[0]);
			return false;
		}
		return true;
	}
	return false;
}

QByteArrayList JSControlCommand::acceptedCommands()
{
	return QByteArrayList()<<"js_start"<<"js_stop"<<"js_restart"<<"js_list";
}
