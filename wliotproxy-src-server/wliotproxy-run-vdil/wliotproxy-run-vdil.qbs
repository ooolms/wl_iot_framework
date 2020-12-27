import qbs
import qbs.Probes

CppApplication
{
	Depends {name: "Qt"; submodules: ["core"]}
	Depends {name: "libwliotproxy-static"}
	Depends {name: "libVDIL-core-blocks"}

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
		name: "wliotproxy-run-vdil app"
		fileTagsFilter: product.type
		qbs.install: true
		qbs.installDir: "usr/sbin"
	}

	files:[
        "../wliotproxyd/AccessManagement/AccessMgr.cpp",
        "../wliotproxyd/AccessManagement/AccessMgr.h",
        "../wliotproxyd/AccessManagement/AccessPolicyTypes.h",
        "../wliotproxyd/CmdArgParser.cpp",
        "../wliotproxyd/CmdArgParser.h",
        "../wliotproxyd/CustomNetworkProxyFactory.cpp",
        "../wliotproxyd/CustomNetworkProxyFactory.h",
        "../wliotproxyd/MainServerConfig.cpp",
        "../wliotproxyd/MainServerConfig.h",
        "../wliotproxyd/Processing/BaseProgramConfigDb.cpp",
        "../wliotproxyd/Processing/BaseProgramConfigDb.h",
        "../wliotproxyd/VDILProcessing/VDILProgramConfigDb.cpp",
        "../wliotproxyd/VDILProcessing/VDILProgramConfigDb.h",
        "EngineCallbacks.cpp",
        "EngineCallbacks.h",
        "EngineHelper.cpp",
        "EngineHelper.h",
        "EngineRun.cpp",
        "EngineRun.h",
        "TimersThread.cpp",
        "TimersThread.h",
        "main.cpp",
    ]
}
