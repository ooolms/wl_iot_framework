import qbs

CppApplication
{
	Depends {name: "Qt"; submodules: ["core","script"]}
	Depends {name: "libwliotproxy"}
	cpp.includePaths: ["../wliotproxyd","../wliotproxyd/Processing"]

	Group
	{
		name: "wliotproxy-run-js app"
		fileTagsFilter: product.type
		qbs.install: true
		qbs.installDir: "usr/sbin"
	}

	files:[
        "../wliotproxyd/CmdArgParser.cpp",
        "../wliotproxyd/CmdArgParser.h",
        "JSDevice.cpp",
        "JSDevice.h",
        "JSDevicesList.cpp",
        "JSDevicesList.h",
        "JSISensorStorage.cpp",
        "JSISensorStorage.h",
        "JSSensorValue.cpp",
        "JSSensorValue.h",
        "JSSessionsStorage.cpp",
        "JSSessionsStorage.h",
        "JSStoragesDatabase.cpp",
        "JSStoragesDatabase.h",
        "main.cpp",
        "JSExtensions/JSConsole.cpp",
        "JSExtensions/JSConsole.h",
        "JSExtensions/JSTimers.cpp",
        "JSExtensions/JSTimers.h",
    ]
}
