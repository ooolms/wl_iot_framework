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

JSControlCommand::JSControlCommand(ARpcOutsideDevice *d,IotProxyCommandProcessor *p)
	:ICommand(d,p)
{
}

bool JSControlCommand::processCommand(const ARpcMessage &m,QByteArrayList &retVal)
{
	if(!acceptedCommands().contains(m.title))return false;
	if(m.title=="js_list")
	{
		QStringList progs=IotProxyInstance::inst().jsPrograms();
		for(const QString &s:progs)
			retVal.append(s.toUtf8());
		return true;
	}
	if(m.args.count()<1)
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QString script=QString::fromUtf8(m.args[0]);
	if(m.title=="js_start")
	{
		if(!IotProxyInstance::inst().controlJSProgram(script,true))
		{
			retVal.append("no script found: "+m.args[0]);
			return false;
		}
		return true;
	}
	else if(m.title=="js_stop")
	{
		if(!IotProxyInstance::inst().controlJSProgram(script,false))
		{
			retVal.append("no script found: "+m.args[0]);
			return false;
		}
		return true;
	}
	else if(m.title=="js_restart")
	{
		if(!IotProxyInstance::inst().controlJSProgram(script,false))
		{
			retVal.append("no script found: "+m.args[0]);
			return false;
		}
		if(!IotProxyInstance::inst().controlJSProgram(script,true))
		{
			retVal.append("no script found: "+m.args[0]);
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
