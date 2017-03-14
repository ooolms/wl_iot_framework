import qbs

CppApplication
{
	Depends {name: "QtUnitLib"}
	Depends {name: "ArduinoRpcLib"}
	Depends {name: "ArduinoRpcUiLib"}
	Depends {name: "Qt";submodules: ["core"]}

	files:[
        "ARpcControlsParsingTests.cpp",
        "ARpcControlsParsingTests.h",
        "ARpcDBDriverFixedBlocksTest.cpp",
        "ARpcDBDriverFixedBlocksTest.h",
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
        "FakeDevice.cpp",
        "FakeDevice.h",
        "main.cpp",
    ]
}
