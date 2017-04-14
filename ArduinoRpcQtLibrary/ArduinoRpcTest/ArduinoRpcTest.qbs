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
        "ARpcDBDriverChainedBlocksTest.cpp",
        "ARpcDBDriverChainedBlocksTest.h",
        "ARpcDBDriverFixedBlocksTest.cpp",
        "ARpcDBDriverFixedBlocksTest.h",
        "ARpcDBDriverHelperTests.cpp",
        "ARpcDBDriverHelperTests.h",
        "ARpcMessageParserTests.cpp",
        "ARpcMessageParserTests.h",
        "ARpcSensorValuesTest.cpp",
        "ARpcSensorValuesTest.h",
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
