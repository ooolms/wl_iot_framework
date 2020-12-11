#include "ARpcTimerOnMillisTest.h"
#include <QThread>

void onTimer(void *d)
{
	++(*((int*)d));
}

ARpcTimerOnMillisTest::ARpcTimerOnMillisTest(QObject *parent)
	:QtUnitTestSet("ARpcTimerOnMillisTest",parent)
{
	addTest((TestFunction)&ARpcTimerOnMillisTest::testSingleTimer,"Test single timer");
	addTest((TestFunction)&ARpcTimerOnMillisTest::testRepeatedTimer,"Test repeated timer");
	addTest((TestFunction)&ARpcTimerOnMillisTest::testGetByIndex,"Test get timer by index");
}

void ARpcTimerOnMillisTest::testSingleTimer()
{
	testTimer(false);
}

void ARpcTimerOnMillisTest::testRepeatedTimer()
{
	testTimer(true);
}

void ARpcTimerOnMillisTest::testGetByIndex()
{
	ARpcTimerOnMillis timer;
	auto e1=timer.addEvent(&onTimer,0);
	auto e2=timer.addEvent(&onTimer,0);
	COMPARE(e1,timer.eventAt(0))
	COMPARE(e2,timer.eventAt(1))
}

void ARpcTimerOnMillisTest::testTimer(bool rep)
{
	ARpcTimerOnMillis timer;
	int cnt=0;
	auto e=timer.addEvent(&onTimer,&cnt);
	if(rep)
		e->repeat(100);
	else e->single(100);
	for(int i=0;i<5;++i)
	{
		QThread::msleep(10);
		timer.loop();
	}
	COMPARE(cnt,0)
	for(int i=0;i<7;++i)
	{
		QThread::msleep(10);
		timer.loop();
	}
	COMPARE(cnt,1)
	for(int i=0;i<12;++i)
	{
		QThread::msleep(10);
		timer.loop();
	}
	if(rep)
		COMPARE(cnt,2)
	else COMPARE(cnt,1)
}
