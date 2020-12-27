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
	Depends {name: "Qt"; submodules: ["network","serialport","script","xml"]}
	Depends {name: "libwliotproxy-base"}
	Depends {name: "libwliotproxyd"}
	Depends {name: "libVDIL"}
	Depends {name: "libVDIL-core-blocks"}
	cpp.includePaths: ["/usr/include"]
	cpp.defines:
	{
		var r=["QT_RESTRICTED_CAST_FROM_ASCII"];
		if(qbs.buildVariant=="debug")
			r.push("DEBUG_BUILD");
		return r;
	}

	Probes.PkgConfigProbe
	{
		id: log4cpp
		name: "log4cpp"
	}

	//	Probes.PkgConfigProbe {id: libusb; name: "libusb-1.0" }
	cpp.linkerFlags:
	{
		//.concat(libusb.libs)
		return [].concat(log4cpp.libs).filter(function(el)
		{
			return el!=null&&el.length!=0&&
				el!="-Wl,--export-dynamic"&&el!="-pthread";//HACK!!
		});
	}

	cpp.dynamicLibraries: ["log4cpp"]

	cpp.cFlags:
	{
		//.concat(libusb.cflags)
		return [].concat(log4cpp.cflags).filter(function(el)
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

	Group
	{
		name: "installing files"
		condition: false
		files:[
         "to_install/etc/wliotproxyd.ini",
         "to_install/usr/lib/systemd/system/wliotproxyd.service",
         "to_install/var/lib/wliotproxyd/devices.ini",
     ]
	}

	Group
	{
		name: "remove later"
		condition: false
		files:[
			"Processing/BaseProgramEngine.cpp",
			"Processing/BaseProgramEngine.h",
			"VDILProcessing/VDILEngine.cpp",
			"VDILProcessing/VDILEngine.h",
			"VDILProcessing/VDILEngineCallbacks.cpp",
			"VDILProcessing/VDILEngineCallbacks.h",
			"VDILProcessing/VDILEngineHelper.cpp",
			"VDILProcessing/VDILEngineHelper.h",
			"VDILProcessing/VDILProgramThread.cpp",
			"VDILProcessing/VDILProgramThread.h",
			"VDILProcessing/VDILTimersThread.cpp",
			"VDILProcessing/VDILTimersThread.h",
			"JSProcessing/JSContinuousStorage.cpp",
			"JSProcessing/JSContinuousStorage.h",
			"JSProcessing/JSDevice.cpp",
			"JSProcessing/JSDevice.h",
			"JSProcessing/JSDevicesList.cpp",
			"JSProcessing/JSDevicesList.h",
			"JSProcessing/JSEngine.cpp",
			"JSProcessing/JSEngine.h",
			"JSProcessing/JSISensorStorage.cpp",
			"JSProcessing/JSISensorStorage.h",
			"JSProcessing/JSLocalDatabase.cpp",
			"JSProcessing/JSLocalDatabase.h",
			"JSProcessing/JSSensorValue.cpp",
			"JSProcessing/JSSensorValue.h",
			"JSProcessing/JSSessionsStorage.cpp",
			"JSProcessing/JSSessionsStorage.h",
			"JSProcessing/JSThread.cpp",
			"JSProcessing/JSThread.h",
			"JSProcessing/JSVirtualDevice.cpp",
			"JSProcessing/JSVirtualDevice.h",
		]
	}

	files:[
        "AccessManagement/AccessMgr.cpp",
        "AccessManagement/AccessMgr.h",
        "AccessManagement/AccessPolicyTypes.h",
        "JSProcessing/JSProgramConfigDb.cpp",
        "JSProcessing/JSProgramConfigDb.h",
        "JSProcessing/JSScriptsManager.cpp",
        "JSProcessing/JSScriptsManager.h",
        "MainServerConfig.h",
        "Processing/BaseProgramConfigDb.cpp",
        "Processing/BaseProgramConfigDb.h",
        "Processing/BaseProgramsManager.cpp",
        "Processing/BaseProgramsManager.h",
        "CmdArgParser.cpp",
        "CmdArgParser.h",
        "CommandProcessor.cpp",
        "CommandProcessor.h",
        "Commands/AccessCommand.cpp",
        "Commands/AccessCommand.h",
        "Commands/AlterozoomAuthCommand.cpp",
        "Commands/AlterozoomAuthCommand.h",
        "Commands/AvailableDataExportServicesCommand.cpp",
        "Commands/AvailableDataExportServicesCommand.h",
        "Commands/BaseProgramsControlCommand.cpp",
        "Commands/BaseProgramsControlCommand.h",
        "Commands/DevNamesCommand.cpp",
        "Commands/DevNamesCommand.h",
        "Commands/DeviceIdCommand.cpp",
        "Commands/DeviceIdCommand.h",
        "Commands/DevicesConfigCommand.cpp",
        "Commands/DevicesConfigCommand.h",
        "Commands/ExecDeviceCommandCommand.cpp",
        "Commands/ExecDeviceCommandCommand.h",
        "Commands/VDILControlCommand.cpp",
        "Commands/VDILControlCommand.h",
        "Commands/GetDevStateCommand.cpp",
        "Commands/GetDevStateCommand.h",
        "Commands/GetSamplesCommand.cpp",
        "Commands/GetSamplesCommand.h",
        "Commands/ICommand.cpp",
        "Commands/ICommand.h",
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
        "Commands/TerminateCommand.cpp",
        "Commands/TerminateCommand.h",
        "Commands/TtyCommands.cpp",
        "Commands/TtyCommands.h",
        "CustomNetworkProxyFactory.cpp",
        "CustomNetworkProxyFactory.h",
        "DataCollectionUnit.cpp",
        "DataCollectionUnit.h",
        "DeviceTypesPriority.cpp",
        "DeviceTypesPriority.h",
        "Devices.cpp",
        "Devices.h",
        "ExternServices/AlterozoomApi.cpp",
        "ExternServices/AlterozoomApi.h",
        "ExternServices/AlterozoomAuthTypes.cpp",
        "ExternServices/AlterozoomAuthTypes.h",
        "ExternServices/AlterozoomAuthentificationStorage.cpp",
        "ExternServices/AlterozoomAuthentificationStorage.h",
        "ExternServices/AlterozoomNoCookieJar.cpp",
        "ExternServices/AlterozoomNoCookieJar.h",
        "ExternServices/AlterozoomSensorDataTranslator.cpp",
        "ExternServices/AlterozoomSensorDataTranslator.h",
        "ExternServices/IotkitAgentCommandSource.cpp",
        "ExternServices/IotkitAgentCommandSource.h",
        "ExternServices/IotkitAgentSensorDataTranslator.cpp",
        "ExternServices/IotkitAgentSensorDataTranslator.h",
        "ExternServices/ThingsSpeakSensorDataTranslator.cpp",
        "ExternServices/ThingsSpeakSensorDataTranslator.h",
        "ServerLogs.cpp",
        "ServerLogs.h",
        "VDILProcessing/VDILProgramConfigDb.cpp",
        "VDILProcessing/VDILProgramConfigDb.h",
        "VDILProcessing/VDILProgramsManager.cpp",
        "VDILProcessing/VDILProgramsManager.h",
        "IExternCommandSource.cpp",
        "IExternCommandSource.h",
        "ISensorDataTranslator.cpp",
        "ISensorDataTranslator.h",
        "LsTtyUsbDevices.cpp",
        "LsTtyUsbDevices.h",
        "MainServerConfig.cpp",
        "ServerInstance.cpp",
        "ServerInstance.h",
        "SslServer.cpp",
        "SslServer.h",
        "StdQFile.cpp",
        "StdQFile.h",
        "TcpControlSocket.cpp",
        "TcpControlSocket.h",
        "TcpDeviceDetect.cpp",
        "TcpDeviceDetect.h",
        "TcpDeviceDetectServer.cpp",
        "TcpDeviceDetectServer.h",
        "UdpDataExport.cpp",
        "UdpDataExport.h",
        "UnixControlSocket.cpp",
        "UnixControlSocket.h",
        "main.cpp",
    ]


//	Group
//	{
//		condition: installConfigs
//		name: "files to install"
//		qbs.install: true
//		qbs.installDir: "/"
//		qbs.installSourceBase: "to_install"
//		files: [
//         "to_install/**/",
//     ]
//	}
}
