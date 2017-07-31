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

CppApplication
{
	name: "wliotproxy"
	Depends {name: "Qt"; submodules: ["network","xml"]}
	Depends {name: "ArduinoRpcLib"}

	files:[
		"CmdArgParser.cpp",
		"CmdArgParser.h",
		"Commands/AddSensorCommand.cpp",
		"Commands/AddSensorCommand.h",
		"Commands/BindSensorCommand.cpp",
		"Commands/BindSensorCommand.h",
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
		"Resources/client.qrc",
		"ShowHelp.cpp",
		"ShowHelp.h",
		"StdQFile.cpp",
		"StdQFile.h",
		"help/help.qrc",
		"main.cpp",
	]
}
