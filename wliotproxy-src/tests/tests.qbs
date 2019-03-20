import qbs

CppApplication
{
	Depends {name: "Qt"; submodules: ["core","widgets"]}
	Depends {name: "libwliotproxy-base"}
	Depends {name: "libwliotproxy-ui"}
	Depends {name: "libwliotproxyd"}
	Depends {name: "libwliotproxy"}
	Depends {name: "QtUnitLib"}
	cpp.includePaths:[
		"../libwliotproxyd/include-private"
	]

	files:[
        "ContinuousStorageTests.cpp",
        "ContinuousStorageTests.h",
        "ControlsParsingTests.cpp",
        "ControlsParsingTests.h",
        "DBDriverChainedBlocksTests.cpp",
        "DBDriverChainedBlocksTests.h",
        "DBDriverFixedBlocksTests.cpp",
        "DBDriverFixedBlocksTests.h",
        "DBDriverHelpersTests.cpp",
        "DBDriverHelpersTests.h",
        "FakeDevice.cpp",
        "FakeDevice.h",
        "LastNValuesStorageTests.cpp",
        "LastNValuesStorageTests.h",
        "MessageTests.cpp",
        "MessageTests.h",
        "RemoveDirRecusive.cpp",
        "RemoveDirRecusive.h",
        "SensorValuesTests.cpp",
        "SensorValuesTests.h",
        "SensorsParsingTests.cpp",
        "SensorsParsingTests.h",
        "SessionStorageTests.cpp",
        "SessionStorageTests.h",
        "SimpleAPITests.cpp",
        "SimpleAPITests.h",
        "StreamParserTests.cpp",
        "StreamParserTests.h",
        "TestData.cpp",
        "TestData.h",
        "TtyWatcherTests.cpp",
        "TtyWatcherTests.h",
        "main.cpp",
    ]
}
