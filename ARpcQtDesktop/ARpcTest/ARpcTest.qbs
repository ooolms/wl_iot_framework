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
	Depends {name: "QtUnitLib"}
	Depends {name: "ARpcLib"}
	Depends {name: "ARpcUiLib"}
	Depends {name: "Qt";submodules: ["core"]}
	Depends {name: "gcov"}

	files:[
        "ARpcContinuousStorageTests.cpp",
        "ARpcContinuousStorageTests.h",
        "ARpcControlsParsingTests.cpp",
        "ARpcControlsParsingTests.h",
        "ARpcDBDriverChainedBlocksTests.cpp",
        "ARpcDBDriverChainedBlocksTests.h",
        "ARpcDBDriverFixedBlocksTests.cpp",
        "ARpcDBDriverFixedBlocksTests.h",
        "ARpcDBDriverHelpersTests.cpp",
        "ARpcDBDriverHelpersTests.h",
        "ARpcLastNValuesStorageTests.cpp",
        "ARpcLastNValuesStorageTests.h",
        "ARpcSensorValuesTests.cpp",
        "ARpcSensorValuesTests.h",
        "ARpcSensorsParsingTests.cpp",
        "ARpcSensorsParsingTests.h",
        "ARpcSessionStorageTests.cpp",
        "ARpcSessionStorageTests.h",
        "ARpcSimpleAPITests.cpp",
        "ARpcSimpleAPITests.h",
        "ARpcStreamParserTests.cpp",
        "ARpcStreamParserTests.h",
        "ARpcTtyWatcherTests.cpp",
        "ARpcTtyWatcherTests.h",
        "FakeDevice.cpp",
        "FakeDevice.h",
        "RemoveDirRecusive.cpp",
        "RemoveDirRecusive.h",
        "TestData.cpp",
        "TestData.h",
        "main.cpp",
    ]
}
