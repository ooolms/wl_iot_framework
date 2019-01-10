import qbs

CppApplication
{
	Depends {name: "cpp"}
	Depends {name: "Qt"; submodules: ["core","network"]}
	Depends {name: "ARpcLib"}
	cpp.includePaths: ["/usr/include","../wliotproxyd/"]

	files:[
        "../wliotproxyd/CmdArgParser.cpp",
        "../wliotproxyd/CmdArgParser.h",
        "../wliotproxyd/IotProxyConfig.cpp",
        "../wliotproxyd/IotProxyConfig.h",
        "main.cpp",
    ]
}
