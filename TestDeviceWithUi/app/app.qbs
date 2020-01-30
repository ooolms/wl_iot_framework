import qbs

CppApplication
{
	Depends {name: "Qt"; submodules: ["gui","widgets","network","script"]}

	files:[
        "Device.cpp",
        "Device.h",
        "DeviceReactionConfig.cpp",
        "DeviceReactionConfig.h",
        "DeviceState.cpp",
        "DeviceState.h",
        "MainWindow.cpp",
        "MainWindow.h",
        "MainWindow.ui",
        "Message.cpp",
        "Message.h",
        "StreamParser.cpp",
        "StreamParser.h",
        "main.cpp",
    ]
}
