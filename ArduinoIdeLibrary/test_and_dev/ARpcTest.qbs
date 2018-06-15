import qbs

CppApplication
{
	name: "ARpcTest"
	Depends {name: "Qt"; submodules: ["core","gui","widgets"]}
	Depends {name: "QtUnitLib"}
    //cpp.cxxFlags: ["-fprofile-arcs","-ftest-coverage"]
    //cpp.staticLibraries: ["gcov"]
	cpp.includePaths: ["../ARpc/"]

	files:[
        "../ARpc/ARpcConfig.cpp",
        "../ARpc/ARpcConfig.h",
        "../ARpc/ARpcDevice.cpp",
        "../ARpc/ARpcDevice.h",
        "../ARpc/ARpcDeviceState.cpp",
        "../ARpc/ARpcDeviceState.h",
        "../ARpc/ARpcIDevEventsCallback.cpp",
        "../ARpc/ARpcIDevEventsCallback.h",
        "../ARpc/ARpcIMessageCallback.cpp",
        "../ARpc/ARpcIMessageCallback.h",
        "../ARpc/ARpcIWriteCallback.cpp",
        "../ARpc/ARpcIWriteCallback.h",
        "../ARpc/ARpcRealDeviceMessageDispatch.cpp",
        "../ARpc/ARpcRealDeviceMessageDispatch.h",
        "../ARpc/ARpcSrvReady.cpp",
        "../ARpc/ARpcSrvReady.h",
        "../ARpc/ARpcStarNetDevice.cpp",
        "../ARpc/ARpcStarNetDevice.h",
        "../ARpc/ARpcStarNetEndPoint.cpp",
        "../ARpc/ARpcStarNetEndPoint.h",
        "../ARpc/ARpcStarNetEndPointDevice.cpp",
        "../ARpc/ARpcStarNetEndPointDevice.h",
        "../ARpc/ARpcStarNetStreamWriter.cpp",
        "../ARpc/ARpcStarNetStreamWriter.h",
        "../ARpc/ARpcStreamParser.cpp",
        "../ARpc/ARpcStreamParser.h",
        "../ARpc/ARpcStreamWriter.cpp",
        "../ARpc/ARpcStreamWriter.h",
        "../ARpc/ARpcUuid.cpp",
        "../ARpc/ARpcUuid.h",
        "ARpcDeviceStateTest.cpp",
        "ARpcDeviceStateTest.h",
        "ARpcStarNetDeviceTest.cpp",
        "ARpcStarNetDeviceTest.h",
        "ARpcStarNetEndPointTest.cpp",
        "ARpcStarNetEndPointTest.h",
        "ARpcStreamParserTest.cpp",
        "ARpcStreamParserTest.h",
        "ARpcUuidTest.cpp",
        "ARpcUuidTest.h",
        "TestNetWriters.cpp",
        "TestNetWriters.h",
        "main.cpp",
    ]
}
