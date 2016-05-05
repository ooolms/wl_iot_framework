import qbs

StaticLibrary
{
	name: "ArduinoRpcLib"

	Depends {name: "cpp"}
	Depends {name: "Qt"; submodules: ["core","network"]}
	cpp.includePaths: "."

	Export
	{
		Depends {name: "cpp"}
		Depends {name: "Qt"; submodules: ["core"]}
		cpp.includePaths: "."
	}

	files:[
        "ARpcBase/ARpcConfig.cpp",
        "ARpcBase/ARpcConfig.h",
        "ARpcBase/ARpcDevice.cpp",
        "ARpcBase/ARpcDevice.h",
        "ARpcBase/ARpcMessage.cpp",
        "ARpcBase/ARpcMessage.h",
        "ARpcBase/ARpcMessageParser.cpp",
        "ARpcBase/ARpcMessageParser.h",
        "ARpcBase/ARpcStreamParser.cpp",
        "ARpcBase/ARpcStreamParser.h",
        "ARpcSimpleAPI/ARpcSyncCall.cpp",
        "ARpcSimpleAPI/ARpcSyncCall.h",
        "ARpcSimpleAPI/ARpcSyncUnsafeCall.cpp",
        "ARpcSimpleAPI/ARpcSyncUnsafeCall.h",
        "Devices/ARpcTcpDevice.cpp",
        "Devices/ARpcTcpDevice.h",
        "Devices/ARpcTtyDevice.cpp",
        "Devices/ARpcTtyDevice.h",
    ]
}
