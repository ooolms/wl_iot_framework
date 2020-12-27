import qbs
import qbs.Probes

CppApplication
{
	Depends {name: "Qt"; submodules: ["core","script"]}
	Depends {name: "libwliotproxy-static"}

	Probes.PkgConfigProbe
	{
		id: log4cpp
		name: "log4cpp"
	}

	cpp.includePaths: ["../wliotproxyd","../wliotproxyd/Processing"]
	cpp.linkerFlags:
	{
		//.concat(libusb.libs)
		return [].concat(log4cpp.libs).filter(function(el)
		{
			return el!=null&&el.length!=0&&
				el!="-Wl,--export-dynamic"&&el!="-pthread";//HACK!!
		});
	}

	cpp.dynamicLibraries: ["log4cpp"]

	cpp.cFlags:
	{
		//.concat(libusb.cflags)
		return [].concat(log4cpp.cflags).filter(function(el)
		{
			return el!=null&&el.length!=0;
		});
	}

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
