import qbs

CppApplication
{
	name: "wliotproxy"
	Depends {name: "Qt"; submodules: ["network"]}
	Depends {name: "ArduinoRpcLib"}

	files:[
		"CmdArgParser.cpp",
		"CmdArgParser.h",
		"Commands/AddSensorCommand.cpp",
		"Commands/AddSensorCommand.h",
		"Commands/ExecCommandCommand.cpp",
		"Commands/ExecCommandCommand.h",
		"Commands/IdentifyTtyCommand.cpp",
		"Commands/IdentifyTtyCommand.h",
		"Commands/ListSensorsCommand.cpp",
		"Commands/ListSensorsCommand.h",
		"Commands/ListStoragesCommand.cpp",
		"Commands/ListStoragesCommand.h",
		"Commands/ListTtyCommand.cpp",
		"Commands/ListTtyCommand.h",
		"Commands/RemoveSensorCommand.cpp",
		"Commands/RemoveSensorCommand.h",
		"IClientCommand.cpp",
		"IClientCommand.h",
		"IotClientCommandArgsParser.cpp",
		"IotClientCommandArgsParser.h",
		"ShowHelp.cpp",
		"ShowHelp.h",
		"StdQFile.cpp",
		"StdQFile.h",
		"help/help.qrc",
		"main.cpp",
	]
}
