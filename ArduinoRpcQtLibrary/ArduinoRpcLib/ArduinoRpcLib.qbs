import qbs

StaticLibrary
{
	name: "ArduinoRpcLib"

	Depends {name: "cpp"}
	Depends {name: "Qt"; submodules: ["core","network","xml","xmlpatterns"]}
	cpp.includePaths: "."

	Export
	{
		Depends {name: "cpp"}
		Depends {name: "Qt"; submodules: ["core","network","xml","xmlpatterns"]}
		cpp.includePaths: "."
	}

	files:[
        "ARpcBase/ARpcConfig.cpp",
        "ARpcBase/ARpcConfig.h",
        "ARpcBase/ARpcControlsDefinition.cpp",
        "ARpcBase/ARpcControlsDefinition.h",
        "ARpcBase/ARpcDevice.cpp",
        "ARpcBase/ARpcDevice.h",
        "ARpcBase/ARpcCommonRc.cpp",
        "ARpcBase/ARpcCommonRc.h",
        "ARpcBase/ARpcDeviceState.cpp",
        "ARpcBase/ARpcDeviceState.h",
        "ARpcBase/ARpcMessage.cpp",
        "ARpcBase/ARpcMessage.h",
        "ARpcBase/ARpcMessageParser.cpp",
        "ARpcBase/ARpcMessageParser.h",
        "ARpcBase/ARpcSensor.cpp",
        "ARpcBase/ARpcSensor.h",
        "ARpcBase/ARpcSimpleMsgDispatch.cpp",
        "ARpcBase/ARpcSimpleMsgDispatch.h",
        "ARpcBase/ARpcStreamParser.cpp",
        "ARpcBase/ARpcStreamParser.h",
        "ARpcBase/ARpcSyncCall.cpp",
        "ARpcBase/ARpcSyncCall.h",
        "ARpcBase/ARpcUnsafeCall.cpp",
        "ARpcBase/ARpcUnsafeCall.h",
        "ARpcDevices/ARpcTcpDevice.cpp",
        "ARpcDevices/ARpcTcpDevice.h",
        "ARpcDevices/ARpcTtyDevice.cpp",
        "ARpcDevices/ARpcTtyDevice.h",
        "ARpcLocalStorage/ARpcLocalDatabase.cpp",
        "ARpcLocalStorage/ARpcLocalDatabase.h",
        "ARpcLocalStorage/ARpcSensorStorage.cpp",
        "ARpcLocalStorage/ARpcSensorStorage.h",
        "Resources/ARpcCommon.qrc",
    ]
}
