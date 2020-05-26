import qbs

StaticLibrary
{
	Depends {name: "cpp"}
	Depends {name: "Qt"; submodules: ["core","network","xml"]}
	Depends {name: "libwliotproxy-base"}
	Depends {name: "libGDIL"}
	targetName: "wliotproxy"
	cpp.includePaths: ["./include","./include-private"]

	Export
	{
		Depends {name: "cpp"}
		Depends {name: "Qt"; submodules: ["core","network","xml"]}
		Depends {name: "libwliotproxy-base"}
		Depends {name: "libGDIL"}
		cpp.includePaths: "./include"
	}

	files:[
        "include-private/ServerConnectionSocketWrap.h",
        "include-private/ServerDeviceBackend.h",
        "include-private/ServerSessionStorage.h",
        "include-private/ServerStorage.h",
        "include/wliot/client/AccessPolicyCommands.h",
        "include/wliot/client/AllServerCommands.h",
        "include/wliot/client/BaseProgramsControlCommands.h",
        "include/wliot/client/BaseProgramsManager.h",
        "include/wliot/client/ConfigurationCommands.h",
        "include/wliot/client/DevicesCommands.h",
        "include/wliot/client/DevicesList.h",
        "include/wliot/client/GDILPrograms.h",
        "include/wliot/client/GDILProgramsCommands.h",
        "include/wliot/client/IBaseProgramsManager.h",
        "include/wliot/client/IDevicesSource.h",
        "include/wliot/client/IGDILProgramsManager.h",
        "include/wliot/client/IJSScriptsManager.h",
        "include/wliot/client/JSScripts.h",
        "include/wliot/client/JSScriptsCommands.h",
        "include/wliot/client/ServerCommandCall.h",
        "include/wliot/client/ServerConnection.h",
        "include/wliot/client/ServerDataTypes.h",
        "include/wliot/client/ServerInstance.h",
        "include/wliot/client/StoragesCommands.h",
        "include/wliot/client/StoragesDatabase.h",
        "include/wliot/client/VirtualDeviceCallback.h",
        "include/wliot/client/VirtualDeviceClient.h",
        "src/AccessPolicyCommands.cpp",
        "src/AllServerCommands.cpp",
        "src/BaseProgramsControlCommands.cpp",
        "src/BaseProgramsManager.cpp",
        "src/ConfigurationCommands.cpp",
        "src/DevicesCommands.cpp",
        "src/DevicesList.cpp",
        "src/GDILPrograms.cpp",
        "src/GDILProgramsCommands.cpp",
        "src/IDevicesSource.cpp",
        "src/IGDILProgramsManager.cpp",
        "src/IJSScriptsManager.cpp",
        "src/JSScripts.cpp",
        "src/JSScriptsCommands.cpp",
        "src/ServerCommandCall.cpp",
        "src/ServerConnection.cpp",
        "src/ServerConnectionSocketWrap.cpp",
        "src/ServerDataTypes.cpp",
        "src/ServerDeviceBackend.cpp",
        "src/ServerInstance.cpp",
        "src/ServerSessionStorage.cpp",
        "src/ServerStorage.cpp",
        "src/StoragesCommands.cpp",
        "src/StoragesDatabase.cpp",
        "src/VirtualDeviceCallback.cpp",
        "src/VirtualDeviceClient.cpp",
    ]
}
