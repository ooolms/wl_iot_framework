import qbs

CppApplication
{
	name: "wiotproxy-alterozoom-auth"
	Depends {name: "Qt"; submodules: ["core","network","xml"]}
	Depends {name: "ArduinoRpcLib"}
	cpp.includePaths: ["/usr/include","../IotProxyServerApp/"]

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
