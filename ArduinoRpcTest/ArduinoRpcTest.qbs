import qbs

CppApplication
{
	Depends {name: "QtUnitLib"}
	Depends {name: "ArduinoRpcLib"}

	files:[
        "ARpcMessageTests.cpp",
        "ARpcMessageTests.h",
        "main.cpp",
    ]
}
