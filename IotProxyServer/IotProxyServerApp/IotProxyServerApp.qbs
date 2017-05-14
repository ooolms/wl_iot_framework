import qbs
import qbs.Probes as Probes

CppApplication
{
	Depends {name: "Qt"; submodules: ["network","serialport"]}
	Depends {name: "ArduinoRpcLib"}
	Probes.PkgConfigProbe {id: libsyslog; name: "syslog-ng" }
	cpp.linkerFlags: libsyslog.libs
	cpp.cFlags: libsyslog.cflags

	files:[
        "CmdArgParser.cpp",
        "CmdArgParser.h",
        "Commands/ICommand.cpp",
        "Commands/ICommand.h",
        "Commands/TtyCommands.cpp",
        "Commands/TtyCommands.h",
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
