import qbs

CppApplication
{
	Depends {name: "Qt"; submodules: ["core","widgets"]}
	Depends {name: "libwliotproxy-base"}
	Depends {name: "libwliotproxy-ui"}
	Depends {name: "libwliotproxyd"}
	Depends {name: "libwliotproxy"}
	Depends {name: "libVDIL"}
	Depends {name: "libVDIL-core-blocks"}
	Depends {name: "QtUnitLib"}
	cpp.includePaths:[
		"../libwliotproxyd/include-private",
		"."
	]

	files:[
        "FakeDeviceBackend.cpp",
        "FakeDeviceBackend.h",
        "RemoveDirRecusive.cpp",
        "RemoveDirRecusive.h",
        "TestData.cpp",
        "TestData.h",
        "libVDIL/MathExpTest.cpp",
        "libVDIL/MathExpTest.h",
        "libVDIL/TimerBlockTests.cpp",
        "libVDIL/TimerBlockTests.h",
        "libVDIL/XmlParserTests.cpp",
        "libVDIL/XmlParserTests.h",
        "libwliotproxy-base/CommandCallTests.cpp",
        "libwliotproxy-base/CommandCallTests.h",
        "libwliotproxy-base/ControlsParsingTests.cpp",
        "libwliotproxy-base/ControlsParsingTests.h",
        "libwliotproxy-base/MessageTests.cpp",
        "libwliotproxy-base/MessageTests.h",
        "libwliotproxy-base/SensorValuesTests.cpp",
        "libwliotproxy-base/SensorValuesTests.h",
        "libwliotproxy-base/SensorsParsingTests.cpp",
        "libwliotproxy-base/SensorsParsingTests.h",
        "libwliotproxy-base/SimpleMsgDispatchTests.cpp",
        "libwliotproxy-base/SimpleMsgDispatchTests.h",
        "libwliotproxy-base/StreamParserTests.cpp",
        "libwliotproxy-base/StreamParserTests.h",
        "libwliotproxyd/ContinuousStorageTests.cpp",
        "libwliotproxyd/ContinuousStorageTests.h",
        "libwliotproxyd/DBDriverChainedBlocksTests.cpp",
        "libwliotproxyd/DBDriverChainedBlocksTests.h",
        "libwliotproxyd/DBDriverFixedBlocksTests.cpp",
        "libwliotproxyd/DBDriverFixedBlocksTests.h",
        "libwliotproxyd/DBDriverHelpersTests.cpp",
        "libwliotproxyd/DBDriverHelpersTests.h",
        "libwliotproxyd/LastNValuesStorageTests.cpp",
        "libwliotproxyd/LastNValuesStorageTests.h",
        "libwliotproxyd/SessionStorageTests.cpp",
        "libwliotproxyd/SessionStorageTests.h",
        "libwliotproxyd/TtyWatcherTests.cpp",
        "libwliotproxyd/TtyWatcherTests.h",
        "main.cpp",
    ]
}
