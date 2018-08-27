import qbs

CppApplication
{
	name: "wliotproxy-alterozoom-auth"
	Depends {name: "Qt"; submodules: ["core","network","xml"]}
	Depends {name: "ARpcLib"}
	cpp.includePaths: ["/usr/include","../IotProxyServerApp/"]

	Group
	{
		name: "wliotproxy-alterozoom-auth app"
		fileTagsFilter: "application"
		qbs.install: true
		qbs.installDir: "usr/bin"
	}

	files:[
        "../IotProxyServerApp/ExternServices/AlterozoomAuthentificationStorage.cpp",
        "../IotProxyServerApp/ExternServices/AlterozoomAuthentificationStorage.h",
        "main.cpp",
        "../IotProxyServerApp/CmdArgParser.cpp",
        "../IotProxyServerApp/CmdArgParser.h",
        "../IotProxyServerApp/ExternServices/AlterozoomApi.cpp",
        "../IotProxyServerApp/ExternServices/AlterozoomApi.h",
        "../IotProxyServerApp/ExternServices/AlterozoomNoCookieJar.cpp",
        "../IotProxyServerApp/ExternServices/AlterozoomNoCookieJar.h",
    ]
}
