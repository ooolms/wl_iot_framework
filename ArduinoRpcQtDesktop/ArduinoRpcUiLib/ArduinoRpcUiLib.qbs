/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

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
        "ARpcUi_Private/ARpcParamRadio.cpp",
        "ARpcUi_Private/ARpcParamRadio.h",
        "ARpcUi_Private/ARpcParamSelect.cpp",
        "ARpcUi_Private/ARpcParamSelect.h",
        "ARpcUi_Private/ARpcParamSlider.cpp",
        "ARpcUi_Private/ARpcParamSlider.h",
        "ARpcUi_Private/ARpcParamTextEdit.cpp",
        "ARpcUi_Private/ARpcParamTextEdit.h",
    ]
}
