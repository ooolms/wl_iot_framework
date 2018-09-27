import qbs

CppApplication
{
	Depends {name: "wliotproxy-client"}
	Depends {name: "Qt"; submodules: ["core"]}

	files:[
        "main.cpp",
    ]
}
