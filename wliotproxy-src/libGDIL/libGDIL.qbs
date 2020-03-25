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
        "include/GIDL/blocks/AverageCalcBlock.h",
        "include/GIDL/blocks/DimChangeBlock.h",
        "include/GIDL/blocks/NormingBlock.h",
        "include/GIDL/blocks/StaticSourceBlock.h",
        "include/GIDL/blocks/StorageSourceBlock.h",
        "include/GIDL/core/BaseBlock.h",
        "include/GIDL/core/BlockInput.h",
        "include/GIDL/core/BlockOutput.h",
        "include/GIDL/core/DataUnit.h",
        "include/GIDL/core/Engine.h",
        "include/GIDL/core/Program.h",
        "include/GIDL/core/SourceBlock.h",
        "src/blocks/AverageCalcBlock.cpp",
        "src/blocks/DimChangeBlock.cpp",
        "src/blocks/NormingBlock.cpp",
        "src/blocks/StaticSourceBlock.cpp",
        "src/blocks/StorageSourceBlock.cpp",
        "src/core/BaseBlock.cpp",
        "src/core/BlockInput.cpp",
        "src/core/BlockOutput.cpp",
        "src/core/DataUnit.cpp",
        "src/core/Engine.cpp",
        "src/core/Program.cpp",
        "src/core/SourceBlock.cpp",
    ]
}
