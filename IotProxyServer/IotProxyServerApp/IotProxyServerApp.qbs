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
	property bool installConfigs: false

	Depends {name: "Qt"; submodules: ["network","serialport","script"]}
	Depends {name: "ArduinoRpcLib"}

	Probes.PkgConfigProbe {id: libsyslog; name: "syslog-ng" }
	cpp.linkerFlags:
	{
		return [].concat(libsyslog.libs).filter(function(el)
		{
			return el!=null&&el.length!=0&&
				el!="-Wl,--export-dynamic"&&el!="-pthread";//HACK!!
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
		fileTagsFilter: product.type
		qbs.install: true
		qbs.installDir: "usr/sbin"
	}

	files:[
        "ARpcTcpDeviceDetect.cpp",
        "ARpcTcpDeviceDetect.h",
        "ARpcTcpDeviceDetectServer.cpp",
        "ARpcTcpDeviceDetectServer.h",
        "ClientThread.cpp",
        "ClientThread.h",
        "CmdArgParser.cpp",
        "CmdArgParser.h",
        "Commands/BindSensorCommand.cpp",
        "Commands/BindSensorCommand.h",
        "Commands/DevicesConfigCommand.cpp",
        "Commands/DevicesConfigCommand.h",
        "Commands/ExecDeviceCommandCommand.cpp",
        "Commands/ExecDeviceCommandCommand.h",
        "Commands/GetSamplesCommand.cpp",
        "Commands/GetSamplesCommand.h",
        "Commands/ICommand.cpp",
        "Commands/ICommand.h",
        "Commands/IdentifyCommand.cpp",
        "Commands/IdentifyCommand.h",
        "Commands/IdentifyTcpCommand.cpp",
        "Commands/IdentifyTcpCommand.h",
        "Commands/JSControlCommand.cpp",
        "Commands/JSControlCommand.h",
        "Commands/ListControlsCommand.cpp",
        "Commands/ListControlsCommand.h",
        "Commands/ListIdentifiedCommand.cpp",
        "Commands/ListIdentifiedCommand.h",
        "Commands/ListSensorsCommand.cpp",
        "Commands/ListSensorsCommand.h",
        "Commands/RegisterVirtualDeviceCommand.cpp",
        "Commands/RegisterVirtualDeviceCommand.h",
        "Commands/SessionStorageCommands.cpp",
        "Commands/SessionStorageCommands.h",
        "Commands/StandardErrors.cpp",
        "Commands/StandardErrors.h",
        "Commands/StoragesCommands.cpp",
        "Commands/StoragesCommands.h",
        "Commands/SubscribeCommand.cpp",
        "Commands/SubscribeCommand.h",
        "Commands/TtyCommands.cpp",
        "Commands/TtyCommands.h",
        "Commands/VdevMeasCommand.cpp",
        "Commands/VdevMeasCommand.h",
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
        "IotProxyDevices.cpp",
        "IotProxyDevices.h",
        "IotProxyInstance.cpp",
        "IotProxyInstance.h",
        "IotProxyRemoteControlSocket.cpp",
        "IotProxyRemoteControlSocket.h",
        "JSDataProcessing/JSContinuousStorage.cpp",
        "JSDataProcessing/JSContinuousStorage.h",
        "JSDataProcessing/JSDevice.cpp",
        "JSDataProcessing/JSDevice.h",
        "JSDataProcessing/JSDevicesList.cpp",
        "JSDataProcessing/JSDevicesList.h",
        "JSDataProcessing/JSISensorStorage.cpp",
        "JSDataProcessing/JSISensorStorage.h",
        "JSDataProcessing/JSLocalDatabase.cpp",
        "JSDataProcessing/JSLocalDatabase.h",
        "JSDataProcessing/JSSensorDataTranslator.cpp",
        "JSDataProcessing/JSSensorDataTranslator.h",
        "JSDataProcessing/JSSensorValue.cpp",
        "JSDataProcessing/JSSensorValue.h",
        "JSDataProcessing/JSSessionsStorage.cpp",
        "JSDataProcessing/JSSessionsStorage.h",
        "JSDataProcessing/JSThread.cpp",
        "JSDataProcessing/JSThread.h",
        "JSDataProcessing/JSVirtualDevice.cpp",
        "JSDataProcessing/JSVirtualDevice.h",
        "JSExtensions/JSConsole.cpp",
        "JSExtensions/JSConsole.h",
        "JSExtensions/JSTimers.cpp",
        "JSExtensions/JSTimers.h",
        "LsTtyUsbDevices.cpp",
        "LsTtyUsbDevices.h",
        "QSslServer.cpp",
        "QSslServer.h",
        "SysLogWrapper.cpp",
        "SysLogWrapper.h",
        "UdpDataExport.cpp",
        "UdpDataExport.h",
        "main.cpp",
    ]


	Group
	{
		condition: installConfigs
		name: "files to install"
		qbs.install: true
		qbs.installDir: "/"
		qbs.installSourceBase: "to_install"
		files: [
         "to_install/**/",
     ]
	}
}
