import qbs

StaticLibrary
{
	name: "ArduinoRpcLib"

	Depends {name: "cpp"}
	Depends {name: "Qt"; submodules: ["core"]}

	Export
	{
		Depends {name: "cpp"}
		Depends {name: "Qt"; submodules: ["core"]}
		cpp.includePaths: "."
	}

	files:[
        "ARpcConfig.cpp",
        "ARpcConfig.h",
        "ARpcDevice.cpp",
        "ARpcDevice.h",
        "ARpcMessage.cpp",
        "ARpcMessage.h",
        "ARpcStreamParser.cpp",
        "ARpcStreamParser.h",
        "ARpcSyncCall.cpp",
        "ARpcSyncCall.h",
        "ARpcTtyDevice.cpp",
        "ARpcTtyDevice.h",
    ]
}
