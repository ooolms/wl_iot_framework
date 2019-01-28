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
	Depends {name: "cpp"}
	Depends {name: "Qt"; submodules: ["core","network","xml","xmlpatterns","serialport"]}
	cpp.includePaths: [".","/usr/include"]
	cpp.minimumWindowsVersion: "6.1"
	cpp.staticLibraries: {
		if(qbs.targetOS.contains("windows"))
			return base;
		else return ["rt"];
	}

	Export
	{
		Depends {name: "cpp"}
		Depends {name: "Qt"; submodules: ["core","network","xml","xmlpatterns","serialport"]}
		Depends {name: "gcov"}
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
        "ARpcBase/ARpcHubDevice.cpp",
        "ARpcBase/ARpcHubDevice.h",
        "ARpcBase/ARpcIMessageHandler.cpp",
        "ARpcBase/ARpcIMessageHandler.h",
        "ARpcBase/ARpcMessage.cpp",
        "ARpcBase/ARpcMessage.h",
        "ARpcBase/ARpcOutsideDevice.cpp",
        "ARpcBase/ARpcOutsideDevice.h",
        "ARpcBase/ARpcRealDevice.cpp",
        "ARpcBase/ARpcRealDevice.h",
        "ARpcBase/ARpcSensorDef.cpp",
        "ARpcBase/ARpcSensorDef.h",
        "ARpcBase/ARpcSensorValue.cpp",
        "ARpcBase/ARpcSensorValue.h",
        "ARpcBase/ARpcServerConfig.cpp",
        "ARpcBase/ARpcServerConfig.h",
        "ARpcBase/ARpcSimpleMsgDispatch.cpp",
        "ARpcBase/ARpcSimpleMsgDispatch.h",
        "ARpcBase/ARpcStreamParser.cpp",
        "ARpcBase/ARpcStreamParser.h",
        "ARpcBase/ARpcSyncCall.cpp",
        "ARpcBase/ARpcSyncCall.h",
        "ARpcBase/ARpcTextSensorValue.cpp",
        "ARpcBase/ARpcTextSensorValue.h",
        "ARpcBase/ARpcVirtualDevice.cpp",
        "ARpcBase/ARpcVirtualDevice.h",
        "ARpcDevices/ARpcSerialDevice.cpp",
        "ARpcDevices/ARpcSerialDevice.h",
        "ARpcDevices/ARpcSerialDriver.cpp",
        "ARpcDevices/ARpcSerialDriver.h",
        "ARpcDevices/ARpcSerialNotificator.cpp",
        "ARpcDevices/ARpcSerialNotificator.h",
        "ARpcDevices/ARpcTcpDevice.cpp",
        "ARpcDevices/ARpcTcpDevice.h",
        "ARpcDevices/ARpcTcpSslDevice.cpp",
        "ARpcDevices/ARpcTcpSslDevice.h",
        "ARpcStorages/ARpcAllStorages.h",
        "ARpcStorages/ARpcBaseFSSensorStorage.cpp",
        "ARpcStorages/ARpcBaseFSSensorStorage.h",
        "ARpcStorages/ARpcContinuousStorage.cpp",
        "ARpcStorages/ARpcContinuousStorage.h",
        "ARpcStorages/ARpcDBDriverChainedBlocks.cpp",
        "ARpcStorages/ARpcDBDriverChainedBlocks.h",
        "ARpcStorages/ARpcDBDriverFixedBlocks.cpp",
        "ARpcStorages/ARpcDBDriverFixedBlocks.h",
        "ARpcStorages/ARpcDBDriverGTimeIndex.cpp",
        "ARpcStorages/ARpcDBDriverGTimeIndex.h",
        "ARpcStorages/ARpcDBDriverHelpers.cpp",
        "ARpcStorages/ARpcDBDriverHelpers.h",
        "ARpcStorages/ARpcFSSensorStorageHelper.cpp",
        "ARpcStorages/ARpcFSSensorStorageHelper.h",
        "ARpcStorages/ARpcFSStoragesDatabase.cpp",
        "ARpcStorages/ARpcFSStoragesDatabase.h",
        "ARpcStorages/ARpcISensorStorage.cpp",
        "ARpcStorages/ARpcISensorStorage.h",
        "ARpcStorages/ARpcISessionSensorStorage.cpp",
        "ARpcStorages/ARpcISessionSensorStorage.h",
        "ARpcStorages/ARpcIStoragesDatabase.cpp",
        "ARpcStorages/ARpcIStoragesDatabase.h",
        "ARpcStorages/ARpcLastNValuesInMemoryStorage.cpp",
        "ARpcStorages/ARpcLastNValuesInMemoryStorage.h",
        "ARpcStorages/ARpcLastNValuesStorage.cpp",
        "ARpcStorages/ARpcLastNValuesStorage.h",
        "ARpcStorages/ARpcSessionStorage.cpp",
        "ARpcStorages/ARpcSessionStorage.h",
        "ARpcStorages/ARpcStorageId.cpp",
        "ARpcStorages/ARpcStorageId.h",
        "ARpcStorages/VeryBigArray.h",
        "Resources/ARpcCommon.qrc",
    ]
}
