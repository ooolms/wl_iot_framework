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

CppApplication
{
	name: "wliotproxy-ui"
	Depends {name: "Qt"; submodules: ["network","xml","gui","widgets"]}
	Depends {name: "ArduinoRpcLib"}
	Depends {name: "ArduinoRpcUiLib"}

	Group
	{
		name: "wliotproxy-ui app"
		fileTagsFilter: "application"
		qbs.install: true
		qbs.installDir: "usr/bin"
	}

	files:[
        "ARpcOutsideDeviceWrap.cpp",
        "ARpcOutsideDeviceWrap.h",
        "ControlUiWidget.cpp",
        "ControlUiWidget.h",
        "main.cpp",
    ]
}
