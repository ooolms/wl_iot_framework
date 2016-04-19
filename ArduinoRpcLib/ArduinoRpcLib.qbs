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
        "ARpcMessageParser.cpp",
        "ARpcMessageParser.h",
        "ARpcStreamParser.cpp",
        "ARpcStreamParser.h",
        "ARpcSyncCall.cpp",
        "ARpcSyncCall.h",
        "ARpcSyncUnsafeCall.cpp",
        "ARpcSyncUnsafeCall.h",
        "ARpcTtyDevice.cpp",
        "ARpcTtyDevice.h",
    ]
}
