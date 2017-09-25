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

bool JSControlCommand::processCommand(const ARpcMessage &m,QStringList &retVal)
{
	if(!acceptedCommands().contains(m.title))return false;
	if(m.title=="list_js")
	{
		retVal=IotProxyInstance::inst().jsPrograms();
		return true;
	}
	if(m.args.count()<1)
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	if(m.title=="start_js")
	{
		if(!IotProxyInstance::inst().controlJSProgram(m.args[0],true))
		{
			retVal.append("no script found: "+m.args[0]);
			return false;
		}
		return true;
	}
	else if(m.title=="stop_js")
	{
		if(!IotProxyInstance::inst().controlJSProgram(m.args[0],false))
		{
			retVal.append("no script found: "+m.args[0]);
			return false;
		}
		return true;
	}
	else if(m.title=="restart_js")
	{
		if(!IotProxyInstance::inst().controlJSProgram(m.args[0],false))
		{
			retVal.append("no script found: "+m.args[0]);
			return false;
		}
		if(!IotProxyInstance::inst().controlJSProgram(m.args[0],true))
		{
			retVal.append("no script found: "+m.args[0]);
			return false;
		}
		return true;
	}
	return false;
}

QStringList JSControlCommand::acceptedCommands()
{
	return QStringList()<<"start_js"<<"stop_js"<<"restart_js"<<"list_js";
}
