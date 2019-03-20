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
#include "StreamParserTests.h"
#include "TtyWatcherTests.h"
#include "SimpleAPITests.h"
#include "SensorsParsingTests.h"
#include "ControlsParsingTests.h"
#include "DBDriverFixedBlocksTests.h"
#include "DBDriverChainedBlocksTests.h"
#include "DBDriverHelpersTests.h"
#include "SensorValuesTests.h"
#include "ContinuousStorageTests.h"
#include "SessionStorageTests.h"
#include "LastNValuesStorageTests.h"
#include "MessageTests.h"
#include "QtUnitTestCollection.h"
#include "QtUnitMain.h"

class TestsCollection
	:public QtUnit::QtUnitTestCollection
{
public:
	explicit TestsCollection(QObject *parent=0)
		:QtUnitTestCollection("TestsCollection",parent)
	{
		new StreamParserTests(this);
//		new TtyWatcherTests(this);//don't work without device, manual execution
		new SimpleAPITests(this);
		new SensorsParsingTests(this);
		new ControlsParsingTests(this);
		new DBDriverFixedBlocksTests(this);
		new DBDriverChainedBlocksTests(this);
		new SensorValuesTests(this);
		new DBDriverHelpersTests(this);
		new ContinuousStorageTests(this);
		new SessionStorageTests(this);
		new LastNValuesStorageTests(this);
		new MessageTests(this);
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
		return "wliotproxy-tests";
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
