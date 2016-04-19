import qbs

CppApplication
{
	Depends {name: "QtUnitLib"}
	Depends {name: "ArduinoRpcLib"}
	Depends {name: "Qt";submodules: ["core"]}

	files:[
        "ARpcComplexTests.cpp",
        "ARpcComplexTests.h",
        "ARpcMessageParserTests.cpp",
        "ARpcMessageParserTests.h",
        "ARpcStreamParserTests.cpp",
        "ARpcStreamParserTests.h",
        "ARpcTtyWatcherTests.cpp",
        "ARpcTtyWatcherTests.h",
        "main.cpp",
    ]
}
