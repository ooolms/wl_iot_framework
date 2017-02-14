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
        "ARpcBase/ARpcSensorsDescription.cpp",
        "ARpcBase/ARpcSensorsDescription.h",
        "ARpcBase/ARpcStreamParser.cpp",
        "ARpcBase/ARpcStreamParser.h",
        "ARpcSimpleAPI/ARpcSimpleMsgDispatch.cpp",
        "ARpcSimpleAPI/ARpcSimpleMsgDispatch.h",
        "ARpcSimpleAPI/ARpcSyncCall.cpp",
        "ARpcSimpleAPI/ARpcSyncCall.h",
        "ARpcSimpleAPI/ARpcSyncUnsafeCall.cpp",
        "ARpcSimpleAPI/ARpcSyncUnsafeCall.h",
        "ARpcDevices/ARpcTcpDevice.cpp",
        "ARpcDevices/ARpcTcpDevice.h",
        "ARpcDevices/ARpcTtyDevice.cpp",
        "ARpcDevices/ARpcTtyDevice.h",
    ]
}
