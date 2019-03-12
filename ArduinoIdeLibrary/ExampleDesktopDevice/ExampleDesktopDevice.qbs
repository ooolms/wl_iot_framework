import qbs

CppApplication
{
	Depends {name: "Qt"; submodules: ["core","network"]}
	cpp.includePaths: ["../ARpc","/usr/include"]

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
        "../ARpc/ARpcStreamParser.cpp",
        "../ARpc/ARpcStreamParser.h",
        "../ARpc/ARpcStreamWriter.cpp",
        "../ARpc/ARpcStreamWriter.h",
        "../ARpc/ARpcUuid.cpp",
        "../ARpc/ARpcUuid.h",
        "main.cpp",
    ]
}
