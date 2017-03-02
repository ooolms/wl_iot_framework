import qbs

StaticLibrary
{
	name: "ArduinoRpcUiLib"
	Depends {name: "cpp"}
	Depends {name: "Qt"; submodules: ["gui","widgets"]}
	Depends {name: "ArduinoRpcLib"}
	cpp.includePaths: "."

	Export
	{
		Depends {name: "cpp"}
		Depends {name: "Qt"; submodules: ["gui","widgets"]}
		Depends {name: "ArduinoRpcLib"}

		cpp.includePaths: "."
	}

	files:[
		"ARpcUi/ARpcControlUi.cpp",
		"ARpcUi/ARpcControlUi.h",
		"ARpcUi_Private/ARpcControlUiCommand.cpp",
		"ARpcUi_Private/ARpcControlUiCommand.h",
		"ARpcUi_Private/ARpcControlUiElement.cpp",
		"ARpcUi_Private/ARpcControlUiElement.h",
		"ARpcUi_Private/ARpcControlUiGroup.cpp",
		"ARpcUi_Private/ARpcControlUiGroup.h",
		"ARpcUi_Private/ARpcIParamElement.cpp",
		"ARpcUi_Private/ARpcIParamElement.h",
		"ARpcUi_Private/ARpcParamCheckbox.cpp",
		"ARpcUi_Private/ARpcParamCheckbox.h",
		"ARpcUi_Private/ARpcParamDial.cpp",
		"ARpcUi_Private/ARpcParamDial.h",
		"ARpcUi_Private/ARpcParamNull.cpp",
		"ARpcUi_Private/ARpcParamNull.h",
		"ARpcUi_Private/ARpcParamSelect.cpp",
		"ARpcUi_Private/ARpcParamSelect.h",
		"ARpcUi_Private/ARpcParamSlider.cpp",
		"ARpcUi_Private/ARpcParamSlider.h",
		"ARpcUi_Private/ARpcParamTextEdit.cpp",
		"ARpcUi_Private/ARpcParamTextEdit.h",
	]
}
