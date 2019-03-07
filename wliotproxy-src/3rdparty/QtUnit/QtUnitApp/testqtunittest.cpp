#include "testqtunittest.h"
#include <QTimer>
#include <QEventLoop>

using namespace QtUnit;

TestQtUnitTest::TestQtUnitTest(QObject *parent)
	:QtUnitTestSet("TestQtUnitTestSet",parent)
{
	addTest((TestFunction)&TestQtUnitTest::test1,"Test1");
	addTest((TestFunction)&TestQtUnitTest::test2,"Test2");
	addTest((TestFunction)&TestQtUnitTest::test3,"Test3");
	addTest((TestFunction)&TestQtUnitTest::test4,"Test4");
}

void TestQtUnitTest::test1()
{
	sleep(1);
	VERIFY(true);
}

void TestQtUnitTest::test2()
{
	sleep(1);
	COMPARE_MESSAGE(1,1,"test error 1");
}

void TestQtUnitTest::test3()
{
	sleep(1);
	COMPARE(2,2);
}

void TestQtUnitTest::test4()
{
	sleep(1);
	VERIFY_MESSAGE(true,"test error 2");
}

void TestQtUnitTest::sleep(int sec)
{
	QEventLoop loop;
	QTimer::singleShot(sec*1000,&loop,SLOT(quit()));
	loop.exec();
}
