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
import qbs.Probes as Probes

StaticLibrary
{
	Depends {name: "cpp"}
	Depends {name: "Qt"; submodules: ["core","network","xml","xmlpatterns","serialport"]}
	Depends {name: "libwliotproxy-base"}
	cpp.includePaths:["./include","./include-private","/usr/include"]
	targetName: "wliotproxyd"
/*	Probes.PkgConfigProbe {id: libusb; name: "libusb-1.0" }
	cpp.linkerFlags:
	{
		return [].concat(libusb.libs).filter(function(el)
		{
			return el!=null&&el.length!=0&&
				el!="-Wl,--export-dynamic"&&el!="-pthread";//HACK!!
		});
	}
	cpp.cxxFlags:
	{
		return [].concat(libusb.cflags).filter(function(el)
		{
			return el!=null&&el.length!=0;
		});
	}*/
	cpp.minimumWindowsVersion: "6.1"

	Export
	{
		Depends {name: "cpp"}
		Depends {name: "Qt"; submodules: ["core","network","xml","xmlpatterns","serialport"]}
		Depends {name: "libwliotproxy-base"}
		cpp.includePaths: "./include"
		/*cpp.linkerFlags:
		{
			return [].concat(libusb.libs).filter(function(el)
			{
				return el!=null&&el.length!=0&&
					el!="-Wl,--export-dynamic"&&el!="-pthread";//HACK!!
			});
		}
		cpp.cxxFlags:
		{
			return [].concat(libusb.cflags).filter(function(el)
			{
				return el!=null&&el.length!=0;
			});
		}*/
	}

	files:[
        "include-private/DBDriverChainedBlocks.h",
        "include-private/DBDriverFixedBlocks.h",
        "include-private/DBDriverGTimeIndex.h",
        "include-private/DBDriverHelpers.h",
        "include-private/FSSensorStorageHelper.h",
        "include-private/SerialDriver.h",
        "include-private/SerialNotificator.h",
        "include-private/VirtualDeviceBackend.h",
        "include/wliot/FSDevicesNamesDatabase.h",
        "include/wliot/IDevicesNamesDatabase.h",
        "include/wliot/devices/SerialDeviceBackend.h",
        "include/wliot/devices/TcpDeviceBackend.h",
        "include/wliot/devices/TcpSslDeviceBackend.h",
        "include/wliot/devices/VirtualDevice.h",
        "include/wliot/storages/AllStorages.h",
        "include/wliot/storages/BaseFSSensorStorage.h",
        "include/wliot/storages/ContinuousStorage.h",
        "include/wliot/storages/FSStoragesDatabase.h",
        "include/wliot/storages/LastNValuesInMemoryStorage.h",
        "include/wliot/storages/LastNValuesStorage.h",
        "include/wliot/storages/SessionStorage.h",
        "src/BaseFSSensorStorage.cpp",
        "src/ContinuousStorage.cpp",
        "src/DBDriverChainedBlocks.cpp",
        "src/DBDriverFixedBlocks.cpp",
        "src/DBDriverGTimeIndex.cpp",
        "src/DBDriverHelpers.cpp",
        "src/FSDevicesNamesDatabase.cpp",
        "src/FSSensorStorageHelper.cpp",
        "src/FSStoragesDatabase.cpp",
        "src/LastNValuesInMemoryStorage.cpp",
        "src/LastNValuesStorage.cpp",
        "src/SerialDeviceBackend.cpp",
        "src/SerialDriver.cpp",
        "src/SerialNotificator.cpp",
        "src/SessionStorage.cpp",
        "src/TcpDeviceBackend.cpp",
        "src/TcpSslDeviceBackend.cpp",
        "src/VirtualDevice.cpp",
        "src/VirtualDeviceBackend.cpp",
    ]
}
