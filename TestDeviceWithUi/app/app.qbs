import qbs

CppApplication
{
	Depends {name: "Qt"; submodules: ["gui","widgets","network"]}

	cpp.includePaths: ["../../ArduinoIdeLibrary/ARpc/"]

	files:[
        "../../ArduinoIdeLibrary/ARpc/ARpcDevice.cpp",
        "../../ArduinoIdeLibrary/ARpc/ARpcDevice.h",
        "../../ArduinoIdeLibrary/ARpc/ARpcDeviceState.cpp",
        "../../ArduinoIdeLibrary/ARpc/ARpcDeviceState.h",
        "../../ArduinoIdeLibrary/ARpc/ARpcIDevEventsCallback.cpp",
        "../../ArduinoIdeLibrary/ARpc/ARpcIDevEventsCallback.h",
        "../../ArduinoIdeLibrary/ARpc/ARpcIMessageCallback.cpp",
        "../../ArduinoIdeLibrary/ARpc/ARpcIMessageCallback.h",
        "../../ArduinoIdeLibrary/ARpc/ARpcIWriteCallback.cpp",
        "../../ArduinoIdeLibrary/ARpc/ARpcIWriteCallback.h",
        "../../ArduinoIdeLibrary/ARpc/ARpcRealDeviceMessageDispatch.cpp",
        "../../ArduinoIdeLibrary/ARpc/ARpcRealDeviceMessageDispatch.h",
        "../../ArduinoIdeLibrary/ARpc/ARpcSrvReady.cpp",
        "../../ArduinoIdeLibrary/ARpc/ARpcSrvReady.h",
        "../../ArduinoIdeLibrary/ARpc/ARpcStarNetDevice.cpp",
        "../../ArduinoIdeLibrary/ARpc/ARpcStarNetDevice.h",
        "../../ArduinoIdeLibrary/ARpc/ARpcStarNetEndPoint.cpp",
        "../../ArduinoIdeLibrary/ARpc/ARpcStarNetEndPoint.h",
        "../../ArduinoIdeLibrary/ARpc/ARpcStarNetEndPointDevice.cpp",
        "../../ArduinoIdeLibrary/ARpc/ARpcStarNetEndPointDevice.h",
        "../../ArduinoIdeLibrary/ARpc/ARpcStarNetStreamWriter.cpp",
        "../../ArduinoIdeLibrary/ARpc/ARpcStarNetStreamWriter.h",
        "../../ArduinoIdeLibrary/ARpc/ARpcStreamParser.cpp",
        "../../ArduinoIdeLibrary/ARpc/ARpcStreamParser.h",
        "../../ArduinoIdeLibrary/ARpc/ARpcStreamWriter.cpp",
        "../../ArduinoIdeLibrary/ARpc/ARpcStreamWriter.h",
        "../../ArduinoIdeLibrary/ARpc/ARpcUuid.cpp",
        "../../ArduinoIdeLibrary/ARpc/ARpcUuid.h",
        "CommandsReactionConfig.cpp",
        "CommandsReactionConfig.h",
        "Device.cpp",
        "Device.h",
        "MainWindow.cpp",
        "MainWindow.h",
        "MainWindow.ui",
        "main.cpp",
    ]
}
