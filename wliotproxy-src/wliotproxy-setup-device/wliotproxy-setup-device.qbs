import qbs

CppApplication
{
	Depends {name: "Qt"; submodules: ["core"]}
	Depends {name: "libwliotproxyd"}
	cpp.includePaths: ["/usr/include","../wliotproxyd/"]

	Group
	{
		name: "wliotproxy-setup-device app"
		fileTagsFilter: "application"
		qbs.install: true
		qbs.installDir: "usr/bin"
	}

	files:[
        "main.cpp",
		"../wliotproxyd/CmdArgParser.cpp",
        "../wliotproxyd/CmdArgParser.h",
    ]
}
