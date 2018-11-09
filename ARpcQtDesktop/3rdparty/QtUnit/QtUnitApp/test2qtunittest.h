#ifndef TEST2QTUNITTEST_H
#define TEST2QTUNITTEST_H

#include "QtUnitTestSet.h"

class Test2QtUnitTest
	:public QtUnit::QtUnitTestSet
{
public:
	explicit Test2QtUnitTest(QObject *parent=0);
	void test1();
	void test2();
	void test3();
	void test4();

protected:
	void sleep(int sec);
};

#endif // TEST2QTUNITTEST_H
