import qbs

CppApplication
{
	Depends {name: "Qt"; submodules: ["core"]}
	Depends {name: "libwliotproxy-base"}
	Depends {name: "libwliotproxyd"}
	cpp.includePaths: ["/usr/include","../wliotproxyd/"]

	files:[
        "main.cpp",
		"../wliotproxyd/CmdArgParser.cpp",
        "../wliotproxyd/CmdArgParser.h",
    ]
}
