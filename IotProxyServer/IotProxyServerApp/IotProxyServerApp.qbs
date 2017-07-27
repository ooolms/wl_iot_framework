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

	files:[
		"CmdArgParser.cpp",
		"CmdArgParser.h",
		"Commands/ExecDeviceCommandCommand.cpp",
		"Commands/ExecDeviceCommandCommand.h",
		"Commands/ICommand.cpp",
		"Commands/ICommand.h",
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
//		"HidApiWrapper.cpp",
//		"HidApiWrapper.h",
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
		"main.cpp",
	]
}
