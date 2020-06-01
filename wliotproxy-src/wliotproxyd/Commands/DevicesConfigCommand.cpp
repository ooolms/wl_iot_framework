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

#include "DevicesConfigCommand.h"
#include "../ServerInstance.h"
#include "wliot/devices/CommandCall.h"
#include "StandardErrors.h"
#include "../MainServerConfig.h"

DevicesConfigCommand::DevicesConfigCommand(CommandProcessor *p)
	:ICommand(p)
{
}


bool DevicesConfigCommand::processCommand(CallContext &ctx)
{
	if(ctx.args.count()<1)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments());
		return false;
	}
	if(proc->uid()!=rootUid)
	{
		ctx.retVal.append(StandardErrors::accessDenied());
		return false;
	}
	QByteArray subCommand=ctx.args[0];
	if(subCommand=="get_tty_by_port_name")
	{
		ctx.retVal.append(MainServerConfig::ttyPortNames.join(',').toUtf8());
		return true;
	}
	else if(subCommand=="set_tty_by_port_name")
	{
		if(ctx.args.count()<2)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments());
			return false;
		}
		if(!MainServerConfig::setTtyByNameFilters(QString::fromUtf8(ctx.args[1])))
		{
			ctx.retVal.append(StandardErrors::cantWriteDevicesConfig());
			return false;
		}
		ServerInstance::inst().devices()->setupControllers();
		return true;
	}
	else if(subCommand=="get_tty_by_vid_pid")
	{
		ctx.retVal.append(MainServerConfig::dumpTtyVidPidFilters().toUtf8());
		return true;
	}
	else if(subCommand=="set_tty_by_vid_pid")
	{
		if(ctx.args.count()<2)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments());
			return false;
		}
		if(!MainServerConfig::setTtyByVidPidFilters(QString::fromUtf8(ctx.args[1])))
		{
			ctx.retVal.append(StandardErrors::cantWriteDevicesConfig());
			return false;
		}
		ServerInstance::inst().devices()->setupControllers();
		return true;
	}
	else if(subCommand=="get_tcp_by_address")
	{
		ctx.retVal.append(MainServerConfig::tcpAddresses.join(',').toUtf8());
		return true;
	}
	else if(subCommand=="set_tcp_by_address")
	{
		if(ctx.args.count()<2)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments());
			return false;
		}
		if(!MainServerConfig::setTcpByAddressFitlers(QString::fromUtf8(ctx.args[1])))
		{
			ctx.retVal.append(StandardErrors::cantWriteDevicesConfig());
			return false;
		}
		ServerInstance::inst().devices()->setupControllers();
		return true;
	}
	else if(subCommand=="set_detect_tcp_devices")
	{
		if(ctx.args.count()<2)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments());
			return false;
		}
		bool ifDetect;
		if(ctx.args[1]=="true"||ctx.args[1]=="1")
			ifDetect=true;
		else if(ctx.args[1]=="false"||ctx.args[1]=="0")
			ifDetect=false;
		else
		{
			ctx.retVal.append(StandardErrors::invalidAgruments());
			return false;
		}
		if(!MainServerConfig::setDetectTcpDevices(ifDetect))
		{
			ctx.retVal.append(StandardErrors::cantWriteDevicesConfig());
			return false;
		}
		ServerInstance::inst().devices()->setupControllers();
		return true;
	}
	ctx.retVal.append("unknown command for devices configuration");
	return false;
}

QByteArrayList DevicesConfigCommand::acceptedCommands()
{
	return QByteArrayList()<<"devices_config";
}
