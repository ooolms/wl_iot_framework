import qbs

StaticLibrary
{
	name: "ArduinoRpcLib"

	Depends {name: "cpp"}
	Depends {name: "Qt"; submodules: ["core","network","xml","xmlpatterns","serialport"]}
	cpp.includePaths: "."

	Export
	{
		Depends {name: "cpp"}
		Depends {name: "Qt"; submodules: ["core","network","xml","xmlpatterns","serialport"]}
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
		"ARpcBase/ARpcISensorValue.cpp",
		"ARpcBase/ARpcISensorValue.h",
		"ARpcBase/ARpcMessage.cpp",
		"ARpcBase/ARpcMessage.h",
		"ARpcBase/ARpcMessageParser.cpp",
		"ARpcBase/ARpcMessageParser.h",
		"ARpcBase/ARpcPacketSensorValue.cpp",
		"ARpcBase/ARpcPacketSensorValue.h",
		"ARpcBase/ARpcSensor.cpp",
		"ARpcBase/ARpcSensor.h",
		"ARpcBase/ARpcSimpleMsgDispatch.cpp",
		"ARpcBase/ARpcSimpleMsgDispatch.h",
		"ARpcBase/ARpcSingleSensorValue.cpp",
		"ARpcBase/ARpcSingleSensorValue.h",
		"ARpcBase/ARpcStreamParser.cpp",
		"ARpcBase/ARpcStreamParser.h",
		"ARpcBase/ARpcSyncCall.cpp",
		"ARpcBase/ARpcSyncCall.h",
		"ARpcBase/ARpcTextSensorValue.cpp",
		"ARpcBase/ARpcTextSensorValue.h",
		"ARpcBase/ARpcUnsafeCall.cpp",
		"ARpcBase/ARpcUnsafeCall.h",
		"ARpcDevices/ARpcTcpDevice.cpp",
		"ARpcDevices/ARpcTcpDevice.h",
		"ARpcDevices/ARpcTtyDevice.cpp",
		"ARpcDevices/ARpcTtyDevice.h",
		"ARpcLocalStorage/ARpcContinuousStorage.cpp",
		"ARpcLocalStorage/ARpcContinuousStorage.h",
		"ARpcLocalStorage/ARpcDBDriverChainedBlocks.cpp",
		"ARpcLocalStorage/ARpcDBDriverChainedBlocks.h",
		"ARpcLocalStorage/ARpcDBDriverFixedBlocks.cpp",
		"ARpcLocalStorage/ARpcDBDriverFixedBlocks.h",
		"ARpcLocalStorage/ARpcDBDriverHelpers.cpp",
		"ARpcLocalStorage/ARpcDBDriverHelpers.h",
		"ARpcLocalStorage/ARpcLastNValuesStorage.cpp",
		"ARpcLocalStorage/ARpcLastNValuesStorage.h",
		"ARpcLocalStorage/ARpcLocalDatabase.cpp",
		"ARpcLocalStorage/ARpcLocalDatabase.h",
		"ARpcLocalStorage/ARpcISensorStorage.cpp",
		"ARpcLocalStorage/ARpcISensorStorage.h",
		"ARpcLocalStorage/ARpcSessionStorage.cpp",
		"ARpcLocalStorage/ARpcSessionStorage.h",
		"ARpcLocalStorage/VeryBigArray.h",
		"Resources/ARpcCommon.qrc",
	]
}
