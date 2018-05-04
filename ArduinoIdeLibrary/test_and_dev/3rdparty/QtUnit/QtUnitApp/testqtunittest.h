#ifndef TESTQTUNITTEST_H
#define TESTQTUNITTEST_H

#include "QtUnitTestSet.h"

class TestQtUnitTest
	:public QtUnit::QtUnitTestSet
{
public:
	explicit TestQtUnitTest(QObject *parent=0);
	void test1();
	void test2();
	void test3();
	void test4();

protected:
	void sleep(int sec);
};

#endif // TESTQTUNITTEST_H
