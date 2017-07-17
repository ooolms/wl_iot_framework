import qbs
import qbs.Probes as Probes

CppApplication
{
	name: "WLIotProxyServer"
	Depends {name: "Qt"; submodules: ["network","serialport"]}
	Depends {name: "ArduinoRpcLib"}
	Probes.PkgConfigProbe {id: libsyslog; name: "syslog-ng" }
	cpp.linkerFlags: libsyslog.libs
	cpp.cFlags: libsyslog.cflags

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
        "IotProxyCommandProcessor.cpp",
        "IotProxyCommandProcessor.h",
        "IotProxyConfig.cpp",
        "IotProxyConfig.h",
        "IotProxyControlSocket.cpp",
        "IotProxyControlSocket.h",
        "IotProxyInstance.cpp",
        "IotProxyInstance.h",
        "SysLogWrapper.cpp",
        "SysLogWrapper.h",
        "main.cpp",
    ]
}
