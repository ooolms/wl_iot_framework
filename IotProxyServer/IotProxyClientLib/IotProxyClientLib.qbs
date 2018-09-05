import qbs

DynamicLibrary
{
	name: "wliotproxy-client"

	Depends {name: "cpp"}
	Depends {name: "Qt"; submodules: ["core","network","xml"]}
	Depends {name: "ARpcLib"}

	Export
	{
		Depends {name: "cpp"}
		Depends {name: "Qt"; submodules: ["core","network","xml"]}
		Depends {name: "ARpcLib"}
		cpp.includePaths: "."
	}

	files:[
        "IotServer.cpp",
        "IotServer.h",
        "IotServerCommands.cpp",
        "IotServerCommands.h",
        "IotServerConfigurationCommands.cpp",
        "IotServerConfigurationCommands.h",
        "IotServerConnection.cpp",
        "IotServerConnection.h",
        "IotServerDevice.cpp",
        "IotServerDevice.h",
        "IotServerDevices.cpp",
        "IotServerDevices.h",
        "IotServerDevicesCommands.cpp",
        "IotServerDevicesCommands.h",
        "IotServerStorages.cpp",
        "IotServerStorages.h",
        "IotServerStoragesCommands.cpp",
        "IotServerStoragesCommands.h",
        "IotServerTypes.cpp",
        "IotServerTypes.h",
        "RemoteSessionStorage.cpp",
        "RemoteSessionStorage.h",
        "RemoteStorage.cpp",
        "RemoteStorage.h",
    ]
}
