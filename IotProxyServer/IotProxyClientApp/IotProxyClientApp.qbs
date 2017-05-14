import qbs

CppApplication
{
	Depends {name: "Qt"; submodules: ["network"]}
	Depends {name: "ArduinoRpcLib"}

	files:[
        "CmdArgParser.cpp",
        "CmdArgParser.h",
        "CommandNames.cpp",
        "CommandNames.h",
        "IClientCommand.cpp",
        "IClientCommand.h",
        "IotClientCommandArgsParser.cpp",
        "IotClientCommandArgsParser.h",
        "ListTtyClientCommand.cpp",
        "ListTtyClientCommand.h",
        "main.cpp",
    ]
}
