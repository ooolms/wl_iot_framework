#include <QApplication>
#include "ARpcStreamParserTests.h"
#include "ARpcMessageParserTests.h"
#include "ARpcTtyWatcherTests.h"
#include "ARpcSimpleAPITests.h"
#include "ARpcSensorsParsingTests.h"
#include "ARpcControlsParsingTests.h"
#include "QtUnitTestCollection.h"
#include "QtUnitMain.h"

class TestsCollection
	:public QtUnit::QtUnitTestCollection
{
public:
	explicit TestsCollection(QObject *parent=0)
		:QtUnitTestCollection("TestsCollection",parent)
	{
		new ARpcMessageParserTests(this);
		new ARpcStreamParserTests(this);
//		new ARpcTtyWatcherTests(this);//don't work without device, manual execution
//		new ARpcSimpleAPITests(this);//don't work without device
		new ARpcSensorsParsingTests(this);
		new ARpcControlsParsingTests(this);
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