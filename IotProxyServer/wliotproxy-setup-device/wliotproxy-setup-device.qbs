import qbs

CppApplication
{
	Depends {name: "Qt"; submodules: ["core"]}
	Depends {name: "ARpcLib"}
	cpp.includePaths: ["/usr/include","../wliotproxyd/"]

	files:[
        "main.cpp",
		"../wliotproxyd/CmdArgParser.cpp",
        "../wliotproxyd/CmdArgParser.h",
    ]
}
