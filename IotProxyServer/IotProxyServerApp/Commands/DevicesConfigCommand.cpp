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
#include "../IotProxyInstance.h"
#include "ARpcBase/ARpcSyncCall.h"
#include "StandardErrors.h"
#include "../IotProxyConfig.h"

DevicesConfigCommand::DevicesConfigCommand(ARpcOutsideDevice *d,IotProxyCommandProcessor *p)
	:ICommand(d,p)
{
}


bool DevicesConfigCommand::processCommand(const ARpcMessage &m,QByteArrayList &retVal)
{
	if(m.title!="devices_config")return false;
	if(m.args.count()<1)
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray subCommand=m.args[0];
	if(subCommand=="get_tty_by_port_name")
	{
		retVal.append(IotProxyConfig::ttyPortNames.join(',').toUtf8());
		return true;
	}
	else if(subCommand=="set_tty_by_port_name")
	{
		if(m.args.count()<2)
		{
			retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		if(!IotProxyConfig::setTtyByNameFilters(QString::fromUtf8(m.args[1])))
		{
			retVal.append(StandardErrors::cantWriteDevicesConfig);
			return false;
		}
		return true;
	}
	else if(subCommand=="get_tty_by_vid_pid")
	{
		retVal.append(IotProxyConfig::dumpTtyVidPidFilters().toUtf8());
		return true;
	}
	else if(subCommand=="set_tty_by_vid_pid")
	{
		if(m.args.count()<2)
		{
			retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		if(!IotProxyConfig::setTtyByVidPidFilters(QString::fromUtf8(m.args[1])))
		{
			retVal.append(StandardErrors::cantWriteDevicesConfig);
			return false;
		}
		return true;
	}
	else if(subCommand=="get_tcp_by_address")
	{
		retVal.append(IotProxyConfig::tcpAddresses.join(',').toUtf8());
		return true;
	}
	else if(subCommand=="set_tcp_by_address")
	{
		if(m.args.count()<2)
		{
			retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		if(!IotProxyConfig::setTcpByAddressFitlers(QString::fromUtf8(m.args[1])))
		{
			retVal.append(StandardErrors::cantWriteDevicesConfig);
			return false;
		}
		return true;
	}
	else if(subCommand=="set_detect_tcp_devices")
	{
		if(m.args.count()<2)
		{
			retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		bool ifDetect;
		if(m.args[1]=="true"||m.args[1]=="1")
			ifDetect=true;
		else if(m.args[1]=="false"||m.args[1]=="0")
			ifDetect=false;
		else
		{
			retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		if(!IotProxyConfig::setDetectTcpDevices(ifDetect))
		{
			retVal.append(StandardErrors::cantWriteDevicesConfig);
			return false;
		}
		return true;
	}
	retVal.append("unknown command for devices configuration");
	return false;
}

QByteArrayList DevicesConfigCommand::acceptedCommands()
{
	return QByteArrayList()<<"devices_config";
}
