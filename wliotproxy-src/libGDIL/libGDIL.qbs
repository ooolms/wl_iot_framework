import qbs

StaticLibrary
{
	targetName: "GIDL"

	Depends {name: "cpp"}
	Depends {name: "Qt"; submodules: ["core"]}
	Depends {name: "libwliotproxy-base"}
	cpp.includePaths: "./include"

	Export
	{
		Depends {name: "cpp"}
		Depends {name: "libwliotproxy-base"}
		cpp.includePaths: "./include"
	}

	files:[
        "include/GIDL/core/BaseBlock.h",
        "include/GIDL/core/BlockInput.h",
        "include/GIDL/core/BlockOutput.h",
        "include/GIDL/core/DataUnit.h",
        "include/GIDL/core/Engine.h",
        "include/GIDL/core/Program.h",
        "include/GIDL/core/SourceBlock.h",
        "src/core/BaseBlock.cpp",
        "src/core/BlockInput.cpp",
        "src/core/BlockOutput.cpp",
        "src/core/DataUnit.cpp",
        "src/core/Engine.cpp",
        "src/core/Program.cpp",
        "src/core/SourceBlock.cpp",
    ]
}
