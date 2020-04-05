import qbs

StaticLibrary
{
	targetName: "libGDIL-editor"

	Depends {name: "cpp"}
	Depends {name: "Qt"; submodules: ["core","gui","widgets"]}
	Depends {name: "libGDIL"}
	cpp.includePaths: "./include"

	Export
	{
		Depends {name: "cpp"}
		Depends {name: "Qt"; submodules: ["core","gui","widgets"]}
		Depends {name: "libGDIL"}
		cpp.includePaths: "./include"
	}


	files:[
        "include/GDIL/editor/BlockGraphicsItem.h",
        "include/GDIL/editor/BlockGraphicsItemPort.h",
        "include/GDIL/editor/Editor.h",
        "include/GDIL/editor/LinkGraphicsItem.h",
        "src/BlockGraphicsItem.cpp",
        "src/BlockGraphicsItemPort.cpp",
        "src/Editor.cpp",
        "src/LinkGraphicsItem.cpp",
    ]
}
