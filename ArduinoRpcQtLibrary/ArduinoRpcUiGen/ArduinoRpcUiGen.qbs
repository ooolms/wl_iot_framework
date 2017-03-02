import qbs

CppApplication
{
	name: "ArduinoRpcUiGen"
	Depends {name: "cpp"}
	Depends {name: "Qt"; submodules: ["gui","widgets"]}
	Depends {name: "ArduinoRpcUiLib"}

	files:[
        "ControlSettingsEdit.ui",
        "ElementSettingsWidget.cpp",
        "ElementSettingsWidget.h",
        "FakeDevice.cpp",
        "FakeDevice.h",
        "GroupSettingsEdit.ui",
        "MainWindow.cpp",
        "MainWindow.h",
        "MainWindow.ui",
        "ParamSettingsEdit.ui",
        "main.cpp",
    ]
}
