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
	cpp.includePaths: ["./include","/usr/include"]
	cpp.minimumWindowsVersion: "6.1"
	cpp.staticLibraries: {
		if(qbs.targetOS.contains("windows"))
			return base;
		else return ["rt"];
	}
	targetName: "wliotproxy-base"

	Export
	{
		Depends {name: "cpp"}
		Depends {name: "Qt"; submodules: ["core","network","xml","xmlpatterns","serialport"]}
		cpp.includePaths: "./include"
	}

	files:[
        "Resources/wliotcommon.qrc",
        "include/wliot/WLIOTCommonRc.h",
        "include/wliot/WLIOTProtocolDefs.h",
        "include/wliot/WLIOTServerProtocolDefs.h",
        "include/wliot/devices/CommandCall.h",
        "include/wliot/devices/ControlsDefinition.h",
        "include/wliot/devices/DeviceState.h",
        "include/wliot/devices/HubDeviceBackend.h",
        "include/wliot/devices/IHighLevelDeviceBackend.h",
        "include/wliot/devices/ILowLevelDeviceBackend.h",
        "include/wliot/devices/Message.h",
        "include/wliot/devices/RealDevice.h",
        "include/wliot/devices/SensorDef.h",
        "include/wliot/devices/SensorValue.h",
        "include/wliot/devices/SimpleMsgDispatch.h",
        "include/wliot/devices/StdHighLevelDeviceBackend.h",
        "include/wliot/devices/StreamParser.h",
        "include/wliot/storages/ISensorStorage.h",
        "include/wliot/storages/ISessionSensorStorage.h",
        "include/wliot/storages/IStoragesDatabase.h",
        "include/wliot/storages/StorageId.h",
        "include/wliot/storages/VeryBigArray.h",
        "src/WLIOTProtocolDefs.cpp",
        "src/WLIOTServerProtocolDefs.cpp",
        "src/devices/CommandCall.cpp",
        "src/devices/ControlsDefinition.cpp",
        "src/devices/DeviceState.cpp",
        "src/devices/HubDeviceBackend.cpp",
        "src/devices/IHighLevelDeviceBackend.cpp",
        "src/devices/ILowLevelDeviceBackend.cpp",
        "src/devices/Message.cpp",
        "src/devices/RealDevice.cpp",
        "src/devices/SensorDef.cpp",
        "src/devices/SensorValue.cpp",
        "src/devices/SimpleMsgDispatch.cpp",
        "src/devices/StdHighLevelDeviceBackend.cpp",
        "src/devices/StreamParser.cpp",
        "src/devices/WLIOTCommonRc.cpp",
        "src/storages/ISensorStorage.cpp",
        "src/storages/ISessionSensorStorage.cpp",
        "src/storages/IStoragesDatabase.cpp",
        "src/storages/StorageId.cpp",
    ]
}
