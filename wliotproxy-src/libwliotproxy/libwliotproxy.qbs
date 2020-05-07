import qbs

StaticLibrary
{
	Depends {name: "cpp"}
	Depends {name: "Qt"; submodules: ["core","network","xml"]}
	Depends {name: "libwliotproxy-base"}
	Depends {name: "libGDIL"}
	targetName: "wliotproxy"

	Export
	{
		Depends {name: "cpp"}
		Depends {name: "Qt"; submodules: ["core","network","xml"]}
		Depends {name: "libwliotproxy-base"}
		cpp.includePaths: "."
	}

	files:[
        "IotServer.cpp",
        "IotServer.h",
        "IotServerAccessPolicyCommands.cpp",
        "IotServerAccessPolicyCommands.h",
        "IotServerCommandCall.cpp",
        "IotServerCommandCall.h",
        "IotServerCommands.cpp",
        "IotServerCommands.h",
        "IotServerConfigurationCommands.cpp",
        "IotServerConfigurationCommands.h",
        "IotServerConnection.cpp",
        "IotServerConnection.h",
        "IotServerConnectionSocketWrap.cpp",
        "IotServerConnectionSocketWrap.h",
        "IotServerDeviceBackend.cpp",
        "IotServerDeviceBackend.h",
        "IotServerDevices.cpp",
        "IotServerDevices.h",
        "IotServerDevicesCommands.cpp",
        "IotServerDevicesCommands.h",
        "IotServerGDILPrograms.cpp",
        "IotServerGDILPrograms.h",
        "IotServerGDILProgramsCommands.cpp",
        "IotServerGDILProgramsCommands.h",
        "IotServerIDevicesSource.cpp",
        "IotServerIDevicesSource.h",
        "IotServerIGDILProgramsManager.cpp",
        "IotServerIGDILProgramsManager.h",
        "IotServerIJSScriptsManager.cpp",
        "IotServerIJSScriptsManager.h",
        "IotServerJSScripts.cpp",
        "IotServerJSScripts.h",
        "IotServerJSScriptsCommands.cpp",
        "IotServerJSScriptsCommands.h",
        "IotServerSessionStorage.cpp",
        "IotServerSessionStorage.h",
        "IotServerStorage.cpp",
        "IotServerStorage.h",
        "IotServerStoragesCommands.cpp",
        "IotServerStoragesCommands.h",
        "IotServerStoragesDatabase.cpp",
        "IotServerStoragesDatabase.h",
        "IotServerTypes.cpp",
        "IotServerTypes.h",
        "IotServerVirtualDeviceCallback.cpp",
        "IotServerVirtualDeviceCallback.h",
        "IotServerVirtualDeviceClient.cpp",
        "IotServerVirtualDeviceClient.h",
    ]
}
