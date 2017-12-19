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

#include <QApplication>
#include "ARpcStreamParserTests.h"
#include "ARpcTtyWatcherTests.h"
#include "ARpcSimpleAPITests.h"
#include "ARpcSensorsParsingTests.h"
#include "ARpcControlsParsingTests.h"
#include "ARpcDBDriverFixedBlocksTests.h"
#include "ARpcDBDriverChainedBlocksTests.h"
#include "ARpcDBDriverHelpersTests.h"
#include "ARpcSensorValuesTests.h"
#include "ARpcContinuousStorageTests.h"
#include "ARpcSessionStorageTests.h"
#include "ARpcLastNValuesStorageTests.h"
#include "QtUnitTestCollection.h"
#include "QtUnitMain.h"

class TestsCollection
	:public QtUnit::QtUnitTestCollection
{
public:
	explicit TestsCollection(QObject *parent=0)
		:QtUnitTestCollection("TestsCollection",parent)
	{
		new ARpcStreamParserTests(this);
//		new ARpcTtyWatcherTests(this);//don't work without device, manual execution
//		new ARpcSimpleAPITests(this);//don't work without device
		new ARpcSensorsParsingTests(this);
		new ARpcControlsParsingTests(this);
		new ARpcDBDriverFixedBlocksTests(this);
		new ARpcDBDriverChainedBlocksTests(this);
		new ARpcSensorValuesTests(this);
		new ARpcDBDriverHelpersTests(this);
		new ARpcContinuousStorageTests(this);
		new ARpcSessionStorageTests(this);
		new ARpcLastNValuesStorageTests(this);
	}
};

class TestMain
	:public QtUnit::QtUnitMain
{
public:
	TestMain(int argc,char **argv):QtUnitMain(argc,argv){}

protected:
	virtual QtUnit::QtUnitTestCollection* makeCollection()
	{
		return new TestsCollection;
	}

	virtual QString project()
	{
		return "ArduinoRpcTest";
	}

	virtual bool createQAppInstForConsoleApps()
	{
		return true;
	}
};

int main(int argc,char **argv)
{
	TestMain m(argc,argv);
	return m.main();
}
