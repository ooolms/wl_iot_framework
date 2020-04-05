import qbs

CppApplication
{
	Depends {name: "Qt"; submodules: ["core","gui","widgets"]}
	Depends {name: "libGDIL"}
	Depends {name: "libGDIL-editor"}

	files:[
        "StdQFile.cpp",
        "StdQFile.h",
        "main.cpp",
    ]
}
