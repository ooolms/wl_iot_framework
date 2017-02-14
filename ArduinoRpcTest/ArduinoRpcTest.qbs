import qbs

CppApplication
{
	Depends {name: "QtUnitLib"}
	Depends {name: "ArduinoRpcLib"}
	Depends {name: "Qt";submodules: ["core"]}

	files:[
        "ARpcControlsParsingTests.cpp",
        "ARpcControlsParsingTests.h",
        "ARpcMessageParserTests.cpp",
        "ARpcMessageParserTests.h",
        "ARpcSensorsParsingTests.cpp",
        "ARpcSensorsParsingTests.h",
        "ARpcSimpleAPITests.cpp",
        "ARpcSimpleAPITests.h",
        "ARpcStreamParserTests.cpp",
        "ARpcStreamParserTests.h",
        "ARpcTtyWatcherTests.cpp",
        "ARpcTtyWatcherTests.h",
        "main.cpp",
    ]
}
