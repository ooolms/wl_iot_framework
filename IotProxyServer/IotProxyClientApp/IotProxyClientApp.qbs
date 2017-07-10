import qbs

CppApplication
{
	Depends {name: "Qt"; submodules: ["network"]}
	Depends {name: "ArduinoRpcLib"}

	files:[
        "CmdArgParser.cpp",
        "CmdArgParser.h",
        "Commands/IdentifyTtyCommand.cpp",
        "Commands/IdentifyTtyCommand.h",
        "Commands/ListTtyCommand.cpp",
        "Commands/ListTtyCommand.h",
        "IClientCommand.cpp",
        "IClientCommand.h",
        "IotClientCommandArgsParser.cpp",
        "IotClientCommandArgsParser.h",
        "main.cpp",
    ]
}
