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
        "DeviceStateMapEditDialog.cpp",
        "DeviceStateMapEditDialog.h",
        "DeviceStateMapEditDialog.ui",
        "JSTimers.cpp",
        "JSTimers.h",
        "LogManager.cpp",
        "LogManager.h",
        "MainWindow.cpp",
        "MainWindow.h",
        "MainWindow.ui",
        "ManualShowingLayoutItem.cpp",
        "ManualShowingLayoutItem.h",
        "TestDevice.cpp",
        "TestDevice.h",
        "TestDeviceConfig.cpp",
        "TestDeviceConfig.h",
        "TestDeviceState.cpp",
        "TestDeviceState.h",
        "main.cpp",
    ]
}
