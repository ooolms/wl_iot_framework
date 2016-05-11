import qbs

CppApplication
{
	Depends {name: "QtUnitLib"}
	Depends {name: "ArduinoRpcLib"}
	Depends {name: "Qt";submodules: ["core"]}

	files:[
        "ARpcMessageParserTests.cpp",
        "ARpcMessageParserTests.h",
        "ARpcSimpleAPITests.cpp",
        "ARpcSimpleAPITests.h",
        "ARpcStreamParserTests.cpp",
        "ARpcStreamParserTests.h",
        "ARpcTtyWatcherTests.cpp",
        "ARpcTtyWatcherTests.h",
        "main.cpp",
    ]
}
