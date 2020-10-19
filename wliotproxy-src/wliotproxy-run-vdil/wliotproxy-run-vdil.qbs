import qbs
import qbs.Probes

CppApplication
{
	Depends {name: "Qt"; submodules: ["core"]}
	Depends {name: "libwliotproxy"}

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
        "../wliotproxyd/CmdArgParser.cpp",
        "../wliotproxyd/CmdArgParser.h",
        "../wliotproxyd/Processing/BaseProgramConfigDb.cpp",
        "../wliotproxyd/Processing/BaseProgramConfigDb.h",
        "../wliotproxyd/VDILProcessing/VDILProgramConfigDb.cpp",
        "../wliotproxyd/VDILProcessing/VDILProgramConfigDb.h",
        "Engine.cpp",
        "Engine.h",
        "EngineCallbacks.cpp",
        "EngineCallbacks.h",
        "EngineHelper.cpp",
        "EngineHelper.h",
        "TimersThread.cpp",
        "TimersThread.h",
        "main.cpp",
    ]
}
