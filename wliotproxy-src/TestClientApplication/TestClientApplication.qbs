import qbs

CppApplication
{
	Depends {name: "libwliotproxy-static"}
	Depends {name: "Qt"; submodules: ["core"]}

	files:[
		"CmdArgParser.cpp",
		"CmdArgParser.h",
        "main.cpp"
    ]
}
