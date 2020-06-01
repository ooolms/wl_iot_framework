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

Application
{
	Depends {name: "cpp"}
	Depends {name: "Qt"; submodules: ["network","xml"]}
	Depends {name: "libwliotproxy-base"}
	Depends {name: "libwliotproxy"}
	Depends {name: "libGDIL"}
	cpp.includePaths: ["/usr/include","../wliotproxyd/"]
	cpp.defines:
	{
		var r=["QT_RESTRICTED_CAST_FROM_ASCII"];
		if(qbs.buildVariant=="debug")
			r.push("DEBUG");
		return r;
	}

	Group
	{
		name: "wliotproxy app"
		fileTagsFilter: "application"
		qbs.install: true
		qbs.installDir: "usr/bin"
	}

	files:[
        "../wliotproxyd/ExternServices/AlterozoomAuthentificationStorage.cpp",
        "../wliotproxyd/ExternServices/AlterozoomAuthentificationStorage.h",
        "CmdArgParser.cpp",
        "CmdArgParser.h",
        "Commands/AddStorageCommand.cpp",
        "Commands/AddStorageCommand.h",
        "Commands/AlterozoomAuthCommand.cpp",
        "Commands/AlterozoomAuthCommand.h",
        "Commands/ApmCommand.cpp",
        "Commands/ApmCommand.h",
        "Commands/BaseProgramControlCommand.cpp",
        "Commands/BaseProgramControlCommand.h",
        "Commands/DataExportCommand.cpp",
        "Commands/DataExportCommand.h",
        "Commands/DefaultCommand.cpp",
        "Commands/DefaultCommand.h",
        "Commands/ExecCommandCommand.cpp",
        "Commands/ExecCommandCommand.h",
        "Commands/GDILControlCommand.cpp",
        "Commands/GDILControlCommand.h",
        "Commands/GetSamplesCommand.cpp",
        "Commands/GetSamplesCommand.h",
        "Commands/HelperCommand.cpp",
        "Commands/HelperCommand.h",
        "Commands/IdentifyTcpCommand.cpp",
        "Commands/IdentifyTcpCommand.h",
        "Commands/IdentifyTtyCommand.cpp",
        "Commands/IdentifyTtyCommand.h",
        "Commands/JSControlCommand.cpp",
        "Commands/JSControlCommand.h",
        "Commands/ListCommandsCommand.cpp",
        "Commands/ListCommandsCommand.h",
        "Commands/ListIdentifiedCommand.cpp",
        "Commands/ListIdentifiedCommand.h",
        "Commands/ListSensorsCommand.cpp",
        "Commands/ListSensorsCommand.h",
        "Commands/ListStoragesCommand.cpp",
        "Commands/ListStoragesCommand.h",
        "Commands/ListTtyCommand.cpp",
        "Commands/ListTtyCommand.h",
        "Commands/SessionCommand.cpp",
        "Commands/SessionCommand.h",
        "Commands/TerminateCommand.cpp",
        "Commands/TerminateCommand.h",
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
