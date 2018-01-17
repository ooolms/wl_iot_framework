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

StaticLibrary
{
	name: "ArduinoRpcLib"

	Depends {name: "cpp"}
	Depends {name: "Qt"; submodules: ["core","network","xml","xmlpatterns","serialport"]}
	Depends {name: "gcov"}
	cpp.includePaths: "."
	cpp.minimumWindowsVersion: "6.0"

	Export
	{
		Depends {name: "cpp"}
		Depends {name: "Qt"; submodules: ["core","network","xml","xmlpatterns","serialport"]}
		Depends {name: "gcov"}
		cpp.includePaths: "."
		cpp.minimumWindowsVersion: "6.0"
	}

	files:[
        "ARpcBase/ARpcAllSensorValues.h",
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
        "ARpcBase/ARpcHubDevice.cpp",
        "ARpcBase/ARpcHubDevice.h",
        "ARpcBase/ARpcIMessageHandler.cpp",
        "ARpcBase/ARpcIMessageHandler.h",
        "ARpcBase/ARpcISensorValue.cpp",
        "ARpcBase/ARpcISensorValue.h",
        "ARpcBase/ARpcMessage.cpp",
        "ARpcBase/ARpcMessage.h",
        "ARpcBase/ARpcOutsideDevice.cpp",
        "ARpcBase/ARpcOutsideDevice.h",
        "ARpcBase/ARpcPacketSensorValue.cpp",
        "ARpcBase/ARpcPacketSensorValue.h",
        "ARpcBase/ARpcRealDevice.cpp",
        "ARpcBase/ARpcRealDevice.h",
        "ARpcBase/ARpcSensor.cpp",
        "ARpcBase/ARpcSensor.h",
        "ARpcBase/ARpcServerConfig.cpp",
        "ARpcBase/ARpcServerConfig.h",
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
        "ARpcBase/ARpcVirtualDevice.cpp",
        "ARpcBase/ARpcVirtualDevice.h",
        "ARpcDevices/ARpcTcpDevice.cpp",
        "ARpcDevices/ARpcTcpDevice.h",
        "ARpcDevices/ARpcTcpSslDevice.cpp",
        "ARpcDevices/ARpcTcpSslDevice.h",
        "ARpcDevices/ARpcTtyDevice.cpp",
        "ARpcDevices/ARpcTtyDevice.h",
        "ARpcLocalStorage/ARpcAllStorages.h",
        "ARpcLocalStorage/ARpcContinuousStorage.cpp",
        "ARpcLocalStorage/ARpcContinuousStorage.h",
        "ARpcLocalStorage/ARpcDBDriverChainedBlocks.cpp",
        "ARpcLocalStorage/ARpcDBDriverChainedBlocks.h",
        "ARpcLocalStorage/ARpcDBDriverFixedBlocks.cpp",
        "ARpcLocalStorage/ARpcDBDriverFixedBlocks.h",
        "ARpcLocalStorage/ARpcDBDriverGTimeIndex.cpp",
        "ARpcLocalStorage/ARpcDBDriverGTimeIndex.h",
        "ARpcLocalStorage/ARpcDBDriverHelpers.cpp",
        "ARpcLocalStorage/ARpcDBDriverHelpers.h",
        "ARpcLocalStorage/ARpcLastNValuesStorage.cpp",
        "ARpcLocalStorage/ARpcLastNValuesStorage.h",
        "ARpcLocalStorage/ARpcLastValueInMemoryStorage.cpp",
        "ARpcLocalStorage/ARpcLastValueInMemoryStorage.h",
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
