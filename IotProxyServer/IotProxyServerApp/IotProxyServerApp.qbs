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

import qbs
import qbs.Probes as Probes

CppApplication
{
	name: "wliotproxyd"
	Depends {name: "Qt"; submodules: ["network","serialport"]}
	Depends {name: "ArduinoRpcLib"}
	Probes.PkgConfigProbe {id: libsyslog; name: "syslog-ng" }
	cpp.linkerFlags:
	{
		return [].concat(libsyslog.libs).filter(function(el)
		{
			return el!=null&&el.length!=0;
		});
	}
	cpp.cFlags:
	{
		return [].concat(libsyslog.cflags).filter(function(el)
		{
			return el!=null&&el.length!=0;
		});
	}

	Group
	{
		name: "wliotproxyd app"
		fileTagsFilter: "application"
		qbs.install: true
		qbs.installDir: "usr/sbin"
	}

	Group
	{
		name: "other files"
		condition: false
		files:[
         "devices.ini",
         "wliotproxyd.ini",
         "wliotproxyd.service",
     ]
	}

	files:[
		"ClientThread.cpp",
		"ClientThread.h",
		"CmdArgParser.cpp",
		"CmdArgParser.h",
		"Commands/BindSensorCommand.cpp",
		"Commands/BindSensorCommand.h",
		"Commands/ExecDeviceCommandCommand.cpp",
		"Commands/ExecDeviceCommandCommand.h",
		"Commands/ICommand.cpp",
		"Commands/ICommand.h",
		"Commands/ListControlsCommand.cpp",
		"Commands/ListControlsCommand.h",
		"Commands/ListIdentifiedCommand.cpp",
		"Commands/ListIdentifiedCommand.h",
		"Commands/ListSensorsCommand.cpp",
		"Commands/ListSensorsCommand.h",
		"Commands/StandardErrors.cpp",
		"Commands/StandardErrors.h",
		"Commands/StoragesCommands.cpp",
		"Commands/StoragesCommands.h",
		"Commands/TtyCommands.cpp",
		"Commands/TtyCommands.h",
		"DataCollectionUnit.cpp",
		"DataCollectionUnit.h",
		"ExternServices/IotkitAgentCommandSource.cpp",
		"ExternServices/IotkitAgentCommandSource.h",
		"ExternServices/IotkitAgentSensorDataTranslator.cpp",
		"ExternServices/IotkitAgentSensorDataTranslator.h",
		"IExternCommandSource.cpp",
		"IExternCommandSource.h",
		"ISensorDataTranslator.cpp",
		"ISensorDataTranslator.h",
		"IotProxyCommandProcessor.cpp",
		"IotProxyCommandProcessor.h",
		"IotProxyConfig.cpp",
		"IotProxyConfig.h",
		"IotProxyControlSocket.cpp",
		"IotProxyControlSocket.h",
		"IotProxyInstance.cpp",
		"IotProxyInstance.h",
		"LsTtyUsbDevices.cpp",
		"LsTtyUsbDevices.h",
		"SysLogWrapper.cpp",
		"SysLogWrapper.h",
		"UdpDataExport.cpp",
		"UdpDataExport.h",
		"main.cpp",
	]
}
