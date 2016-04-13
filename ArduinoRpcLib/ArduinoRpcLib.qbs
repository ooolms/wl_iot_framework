import qbs

StaticLibrary
{
	name: "ArduinoRpcLib"

	Depends {name: "cpp"}
	Depends {name: "Qt"; submodules: ["core"]}

	Export
	{
		Depends {name: "cpp"}
		Depends {name: "Qt"; submodules: ["core"]}
		cpp.includePaths: "."
	}

	files:[
        "ARpcMessage.cpp",
        "ARpcMessage.h",
    ]
}
