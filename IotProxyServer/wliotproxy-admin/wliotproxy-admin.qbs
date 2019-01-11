import qbs

CppApplication
{
	Depends {name: "cpp"}
	Depends {name: "Qt"; submodules: ["core","network"]}
	Depends {name: "ARpcLib"}
	cpp.includePaths: ["/usr/include","../wliotproxyd/"]

	Group
	{
		name: "wliotproxy-admin app"
		fileTagsFilter: "application"
		qbs.install: true
		qbs.installDir: "usr/sbin"
	}

	files:[
        "../wliotproxyd/CmdArgParser.cpp",
        "../wliotproxyd/CmdArgParser.h",
        "../wliotproxyd/IotProxyAccessMgr.cpp",
        "../wliotproxyd/IotProxyAccessMgr.h",
        "../wliotproxyd/IotProxyConfig.cpp",
        "../wliotproxyd/IotProxyConfig.h",
        "main.cpp",
    ]
}
