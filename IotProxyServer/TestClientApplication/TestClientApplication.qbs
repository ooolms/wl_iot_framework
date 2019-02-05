import qbs

CppApplication
{
	Depends {name: "wliotproxy-client"}
	Depends {name: "Qt"; submodules: ["core"]}
	cpp.includePaths: ["../wliotproxyd"]

	files:[
        "main.cpp",
		"../wliotproxyd/CmdArgParser.cpp",
		"../wliotproxyd/CmdArgParser.h"
    ]
}
