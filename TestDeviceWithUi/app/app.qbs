import qbs

CppApplication
{
	Depends {name: "Qt"; submodules: ["gui","widgets","network","script","xml"]}
	Depends {name: "libwliotproxy-base"}

	files:[
        "CmdArgParser.cpp",
        "CmdArgParser.h",
        "CommandReactionConfigDialog.cpp",
        "CommandReactionConfigDialog.h",
        "CommandReactionConfigDialog.ui",
        "Device.cpp",
        "Device.h",
        "DeviceConfig.cpp",
        "DeviceConfig.h",
        "DeviceState.cpp",
        "DeviceState.h",
        "LogManager.cpp",
        "LogManager.h",
        "MainWindow.cpp",
        "MainWindow.h",
        "MainWindow.ui",
        "ManualShowingLayoutItem.cpp",
        "ManualShowingLayoutItem.h",
        "main.cpp",
    ]
}
