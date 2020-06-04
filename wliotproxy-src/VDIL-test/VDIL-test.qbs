import qbs

CppApplication
{
	Depends {name: "Qt"; submodules: ["core","gui","widgets"]}
	Depends {name: "libVDIL"}
	Depends {name: "libVDIL-editor"}
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
