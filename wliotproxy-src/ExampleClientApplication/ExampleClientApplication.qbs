import qbs

CppApplication
{
	Depends {name: "libwliotproxy"}
	Depends {name: "Qt"; submodules: ["core"]}

	files:[
		"CmdArgParser.cpp",
		"CmdArgParser.h",
        "main.cpp"
    ]
}
