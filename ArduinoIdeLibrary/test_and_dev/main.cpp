#include "QtUnitMain.h"
#include "ARpcStreamParserTest.h"
#include "ARpcUuidTest.h"
#include "ARpcDeviceStateTest.h"
#include "ARpcStarNetDeviceTest.h"
#include "ARpcStarNetEndPointTest.h"
#include "ARpcEEPROMTest.h"
#include "ARpcTimerOnMillisTest.h"
#include <QDebug>

class TestMain
	:public QtUnit::QtUnitMain
{
public:
	TestMain(int argc,char **argv)
		:QtUnitMain(argc,argv){}

protected:
	virtual QtUnit::QtUnitTestCollection* makeCollection()
	{
		QtUnit::QtUnitTestCollection *collection=new QtUnit::QtUnitTestCollection("All tests");
		new ARpcStreamParserTest(collection);
		new ARpcUuidTest(collection);
		new ARpcDeviceStateTest(collection);
		new ARpcStarNetDeviceTest(collection);
		new ARpcStarNetEndPointTest(collection);
		new ARpcEEPROMTest(collection);
		new ARpcTimerOnMillisTest(collection);
		return collection;
	}
	virtual QString project(){return "ARpc test app";}
};

int main(int argc,char **argv)
{
	TestMain m(argc,argv);
	return m.main();
}

//int main(int,char *[])
//{
//	bool ok=testParser();
//	qDebug()<<"\n";
//	ok=ok&&testDeviceState();
//	qDebug()<<"\n";
//	ok=ok&&testUuid();
//	qDebug()<<"\n";
//	ok=ok&&testStarNet();
//	return ok?0:1;
//}
