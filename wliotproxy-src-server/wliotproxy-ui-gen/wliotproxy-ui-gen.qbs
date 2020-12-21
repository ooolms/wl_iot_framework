import qbs

CppApplication
{
	Depends {name: "Qt"; submodules: ["core","widgets"]}
	Depends {name: "libwliotproxy-ui-config"}

	files:[
        "MainWindow.cpp",
        "MainWindow.h",
        "MainWindow.ui",
        "main.cpp",
    ]
}
