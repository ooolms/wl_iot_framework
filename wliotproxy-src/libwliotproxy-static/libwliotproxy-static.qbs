import qbs

StaticLibrary
{
	Depends {name: "cpp"}
	Depends {name: "Qt"; submodules: ["core","network","xml"]}
	Depends {name: "libwliotproxy-base"}
	Depends {name: "libVDIL"}
	targetName: "wliotproxy-static"
	cpp.includePaths: ["../libwliotproxy/include","../libwliotproxy/include-private"]

	Export
	{
		Depends {name: "cpp"}
		Depends {name: "Qt"; submodules: ["core","network","xml"]}
		Depends {name: "libwliotproxy-base"}
		Depends {name: "libVDIL"}
		cpp.includePaths: ["../libwliotproxy/include"]
	}

	files:[
        "../libwliotproxy/include-private/ServerConnectionSocketWrap.h",
        "../libwliotproxy/include-private/ServerDeviceBackend.h",
        "../libwliotproxy/include-private/ServerSessionStorage.h",
        "../libwliotproxy/include-private/ServerStorage.h",
        "../libwliotproxy/include/wliot/client/AccessPolicyCommands.h",
        "../libwliotproxy/include/wliot/client/AllServerCommands.h",
        "../libwliotproxy/include/wliot/client/BaseProgramsControlCommands.h",
        "../libwliotproxy/include/wliot/client/BaseProgramsManager.h",
        "../libwliotproxy/include/wliot/client/ConfigurationCommands.h",
        "../libwliotproxy/include/wliot/client/DevicesCommands.h",
        "../libwliotproxy/include/wliot/client/DevicesList.h",
        "../libwliotproxy/include/wliot/client/IBaseProgramsManager.h",
        "../libwliotproxy/include/wliot/client/IDevicesSource.h",
        "../libwliotproxy/include/wliot/client/IJSScriptsManager.h",
        "../libwliotproxy/include/wliot/client/IVDILProgramsManager.h",
        "../libwliotproxy/include/wliot/client/JSScripts.h",
        "../libwliotproxy/include/wliot/client/JSScriptsCommands.h",
        "../libwliotproxy/include/wliot/client/ServerCommandCall.h",
        "../libwliotproxy/include/wliot/client/ServerConnection.h",
        "../libwliotproxy/include/wliot/client/ServerDataTypes.h",
        "../libwliotproxy/include/wliot/client/ServerInstance.h",
        "../libwliotproxy/include/wliot/client/StoragesCommands.h",
        "../libwliotproxy/include/wliot/client/StoragesDatabase.h",
        "../libwliotproxy/include/wliot/client/VDILPrograms.h",
        "../libwliotproxy/include/wliot/client/VDILProgramsCommands.h",
        "../libwliotproxy/include/wliot/client/VirtualDeviceCallback.h",
        "../libwliotproxy/include/wliot/client/VirtualDeviceClient.h",
        "../libwliotproxy/src/AccessPolicyCommands.cpp",
        "../libwliotproxy/src/AllServerCommands.cpp",
        "../libwliotproxy/src/BaseProgramsControlCommands.cpp",
        "../libwliotproxy/src/BaseProgramsManager.cpp",
        "../libwliotproxy/src/ConfigurationCommands.cpp",
        "../libwliotproxy/src/DevicesCommands.cpp",
        "../libwliotproxy/src/DevicesList.cpp",
        "../libwliotproxy/src/IDevicesSource.cpp",
        "../libwliotproxy/src/IJSScriptsManager.cpp",
        "../libwliotproxy/src/IVDILProgramsManager.cpp",
        "../libwliotproxy/src/JSScripts.cpp",
        "../libwliotproxy/src/JSScriptsCommands.cpp",
        "../libwliotproxy/src/ServerCommandCall.cpp",
        "../libwliotproxy/src/ServerConnection.cpp",
        "../libwliotproxy/src/ServerConnectionSocketWrap.cpp",
        "../libwliotproxy/src/ServerDataTypes.cpp",
        "../libwliotproxy/src/ServerDeviceBackend.cpp",
        "../libwliotproxy/src/ServerInstance.cpp",
        "../libwliotproxy/src/ServerSessionStorage.cpp",
        "../libwliotproxy/src/ServerStorage.cpp",
        "../libwliotproxy/src/StoragesCommands.cpp",
        "../libwliotproxy/src/StoragesDatabase.cpp",
        "../libwliotproxy/src/VDILPrograms.cpp",
        "../libwliotproxy/src/VDILProgramsCommands.cpp",
        "../libwliotproxy/src/VirtualDeviceCallback.cpp",
        "../libwliotproxy/src/VirtualDeviceClient.cpp",
    ]
}
