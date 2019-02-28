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
        "../wliotproxyd/AccessManagement/AccessMgr.cpp",
        "../wliotproxyd/AccessManagement/AccessMgr.h",
        "../wliotproxyd/AccessManagement/AcsMgrBaseTypes.h",
        "../wliotproxyd/CmdArgParser.cpp",
        "../wliotproxyd/CmdArgParser.h",
        "../wliotproxyd/IotProxyConfig.cpp",
        "../wliotproxyd/IotProxyConfig.h",
        "../wliotproxyd/IotProxyNetworkProxyFactory.cpp",
        "../wliotproxyd/IotProxyNetworkProxyFactory.h",
        "main.cpp",
    ]
}
