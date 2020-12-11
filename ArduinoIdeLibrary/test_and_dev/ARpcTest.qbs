import qbs

CppApplication
{
	name: "ARpcTest"
	Depends {name: "Qt"; submodules: ["core","gui","widgets"]}
	Depends {name: "QtUnitLib"}
    //cpp.cxxFlags: ["-fprofile-arcs","-ftest-coverage"]
    //cpp.staticLibraries: ["gcov"]
	cpp.includePaths: ["../ARpc/",".","../ARpcEEPROM/","../ARpcTimerOnMillis/"]
	cpp.defines: ["ARDUINO"]

	Group
	{
		name: "WiFi class"
		condition: false
		files: [
			"../ARpcESP8266WiFiDevice/ARpcESP8266WiFiDevice.cpp",
			"../ARpcESP8266WiFiDevice/ARpcESP8266WiFiDevice.h",
		]
	}

	files:[
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
        "../ARpcEEPROM/ARpcEEPROM.cpp",
        "../ARpcEEPROM/ARpcEEPROM.h",
        "../ARpcTimerOnMillis/ARpcTimerOnMillis.cpp",
        "../ARpcTimerOnMillis/ARpcTimerOnMillis.h",
        "ARpcDeviceStateTest.cpp",
        "ARpcDeviceStateTest.h",
        "ARpcEEPROMTest.cpp",
        "ARpcEEPROMTest.h",
        "ARpcStarNetDeviceTest.cpp",
        "ARpcStarNetDeviceTest.h",
        "ARpcStarNetEndPointTest.cpp",
        "ARpcStarNetEndPointTest.h",
        "ARpcStreamParserTest.cpp",
        "ARpcStreamParserTest.h",
        "ARpcTimerOnMillisTest.cpp",
        "ARpcTimerOnMillisTest.h",
        "ARpcUuidTest.cpp",
        "ARpcUuidTest.h",
        "Arduino.cpp",
        "Arduino.h",
        "EEPROM.cpp",
        "EEPROM.h",
        "TestNetWriters.cpp",
        "TestNetWriters.h",
        "main.cpp",
    ]
}
