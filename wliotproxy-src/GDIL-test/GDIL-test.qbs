import qbs

CppApplication
{
	Depends {name: "Qt"; submodules: ["core","gui","widgets"]}
	Depends {name: "libGDIL"}
	Depends {name: "libGDIL-editor"}
	Depends {name: "libwliotproxy"}

	files:[
        "MainWindow.cpp",
        "MainWindow.h",
        "MainWindow.ui",
        "StdQFile.cpp",
        "StdQFile.h",
        "main.cpp",
    ]
}
