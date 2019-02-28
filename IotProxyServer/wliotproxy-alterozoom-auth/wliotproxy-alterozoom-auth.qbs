import qbs

CppApplication
{
	Depends {name: "Qt"; submodules: ["core","network","xml"]}
	Depends {name: "ARpcLib"}
	cpp.includePaths: ["/usr/include","../wliotproxyd/"]

	Group
	{
		name: "wliotproxy-alterozoom-auth app"
		fileTagsFilter: "application"
		qbs.install: true
		qbs.installDir: "usr/bin"
	}

	files:[
        "../wliotproxyd/AccessManagement/AccessMgr.cpp",
        "../wliotproxyd/AccessManagement/AccessMgr.h",
        "../wliotproxyd/AccessManagement/AcsMgrBaseTypes.h",
        "../wliotproxyd/ExternServices/AlterozoomAuthentificationStorage.cpp",
        "../wliotproxyd/ExternServices/AlterozoomAuthentificationStorage.h",
        "../wliotproxyd/IotProxyConfig.cpp",
        "../wliotproxyd/IotProxyConfig.h",
        "../wliotproxyd/IotProxyNetworkProxyFactory.cpp",
        "../wliotproxyd/IotProxyNetworkProxyFactory.h",
        "main.cpp",
        "../wliotproxyd/CmdArgParser.cpp",
        "../wliotproxyd/CmdArgParser.h",
        "../wliotproxyd/ExternServices/AlterozoomApi.cpp",
        "../wliotproxyd/ExternServices/AlterozoomApi.h",
        "../wliotproxyd/ExternServices/AlterozoomNoCookieJar.cpp",
        "../wliotproxyd/ExternServices/AlterozoomNoCookieJar.h",
    ]
}
