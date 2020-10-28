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
	cpp.cxxFlags: log4cpp.cflags
	cpp.linkerFlags: log4cpp.libs

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
