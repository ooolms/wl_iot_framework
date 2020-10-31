#include "test2qtunittest.h"
#include <QTimer>
#include <QEventLoop>

using namespace QtUnit;

Test2QtUnitTest::Test2QtUnitTest(QObject *parent)
	:QtUnitTestSet("Test2QtUnitTestSet",parent)
{
	addTest((TestFunction)&Test2QtUnitTest::test1,"Test1");
	addTest((TestFunction)&Test2QtUnitTest::test2,"Test2");
	addTest((TestFunction)&Test2QtUnitTest::test3,"Test3");
	addTest((TestFunction)&Test2QtUnitTest::test4,"Test4");
}

void Test2QtUnitTest::test1()
{
	sleep(1);
	VERIFY(false);
}

void Test2QtUnitTest::test2()
{
	sleep(1);
	COMPARE_MESSAGE(1,2,"test error 1");
}

void Test2QtUnitTest::test3()
{
	sleep(1);
	COMPARE(1,2);
}

void Test2QtUnitTest::test4()
{
	sleep(1);
	VERIFY_MESSAGE(false,"test error 2");
}

void Test2QtUnitTest::sleep(int sec)
{
	QEventLoop loop;
	QTimer::singleShot(sec*1000,&loop,SLOT(quit()));
	loop.exec();
}
