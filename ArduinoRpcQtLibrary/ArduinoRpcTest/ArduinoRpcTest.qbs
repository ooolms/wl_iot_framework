import qbs

CppApplication
{
	Depends {name: "QtUnitLib"}
	Depends {name: "ArduinoRpcLib"}
	Depends {name: "ArduinoRpcUiLib"}
	Depends {name: "Qt";submodules: ["core"]}

	files:[
        "ARpcContinuousStorageTests.cpp",
        "ARpcContinuousStorageTests.h",
        "ARpcControlsParsingTests.cpp",
        "ARpcControlsParsingTests.h",
        "ARpcDBDriverChainedBlocksTests.cpp",
        "ARpcDBDriverChainedBlocksTests.h",
        "ARpcDBDriverFixedBlocksTests.cpp",
        "ARpcDBDriverFixedBlocksTests.h",
        "ARpcDBDriverHelpersTests.cpp",
        "ARpcDBDriverHelpersTests.h",
        "ARpcLastNValuesStorageTests.cpp",
        "ARpcLastNValuesStorageTests.h",
        "ARpcMessageParserTests.cpp",
        "ARpcMessageParserTests.h",
        "ARpcSensorValuesTests.cpp",
        "ARpcSensorValuesTests.h",
        "ARpcSensorsParsingTests.cpp",
        "ARpcSensorsParsingTests.h",
        "ARpcSessionStorageTests.cpp",
        "ARpcSessionStorageTests.h",
        "ARpcSimpleAPITests.cpp",
        "ARpcSimpleAPITests.h",
        "ARpcStreamParserTests.cpp",
        "ARpcStreamParserTests.h",
        "ARpcTtyWatcherTests.cpp",
        "ARpcTtyWatcherTests.h",
        "FakeDevice.cpp",
        "FakeDevice.h",
        "RemoveDirRecusive.cpp",
        "RemoveDirRecusive.h",
        "main.cpp",
    ]
}
