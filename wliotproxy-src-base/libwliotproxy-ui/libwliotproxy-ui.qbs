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
	Depends {name: "cpp"}
	Depends {name: "Qt"; submodules: ["gui","widgets"]}
	Depends {name: "libwliotproxy-base"}
	cpp.includePaths: ["./include","./include-private"]

	Export
	{
		Depends {name: "cpp"}
		Depends {name: "Qt"; submodules: ["gui","widgets"]}
		Depends {name: "libwliotproxy-base"}
		cpp.includePaths: "./include"
	}

	files:[
        "include-private/ControlUiCommand.h",
        "include-private/ControlUiElement.h",
        "include-private/ControlUiGroup.h",
        "include-private/IParamElement.h",
        "include-private/ParamCheckbox.h",
        "include-private/ParamDial.h",
        "include-private/ParamHidden.h",
        "include-private/ParamNull.h",
        "include-private/ParamRadio.h",
        "include-private/ParamSelect.h",
        "include-private/ParamSlider.h",
        "include-private/ParamTextEdit.h",
        "include/wliot/ControlUi.h",
        "src/ControlUi.cpp",
        "src/ControlUiCommand.cpp",
        "src/ControlUiElement.cpp",
        "src/ControlUiGroup.cpp",
        "src/IParamElement.cpp",
        "src/ParamCheckbox.cpp",
        "src/ParamDial.cpp",
        "src/ParamHidden.cpp",
        "src/ParamNull.cpp",
        "src/ParamRadio.cpp",
        "src/ParamSelect.cpp",
        "src/ParamSlider.cpp",
        "src/ParamTextEdit.cpp",
    ]
}
