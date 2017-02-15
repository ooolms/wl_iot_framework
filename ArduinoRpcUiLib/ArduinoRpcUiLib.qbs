import qbs

StaticLibrary
{
    files: [
        "ARpcControlUi.cpp",
        "ARpcControlUi.h",
        "ARpcControlUiCommand.cpp",
        "ARpcControlUiCommand.h",
        "ARpcControlUiElement.cpp",
        "ARpcControlUiElement.h",
        "ARpcControlUiGroup.cpp",
        "ARpcControlUiGroup.h",
        "ARpcIParamElement.cpp",
        "ARpcIParamElement.h",
        "ARpcParamCheckbox.cpp",
        "ARpcParamCheckbox.h",
        "ARpcParamDial.cpp",
        "ARpcParamDial.h",
        "ARpcParamNull.cpp",
        "ARpcParamNull.h",
        "ARpcParamSelect.cpp",
        "ARpcParamSelect.h",
        "ARpcParamSlider.cpp",
        "ARpcParamSlider.h",
        "ARpcParamTextEdit.cpp",
        "ARpcParamTextEdit.h",
    ]
	name: "ArduinoRpcUiLib"
	Depends {name: "cpp"}
	Depends {name: "Qt"; submodules: ["gui","widgets"]}
	Depends {name: "ArduinoRpcLib"}

	Export
	{
		Depends {name: "cpp"}
		Depends {name: "Qt"; submodules: ["gui","widgets"]}
		Depends {name: "ArduinoRpcLib"}

		cpp.includePaths: "."
	}
}
