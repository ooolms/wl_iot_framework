#ifndef ARPCTIMERONMILLISTEST_H
#define ARPCTIMERONMILLISTEST_H

#include "QtUnitTestSet.h"
#include "ARpcTimerOnMillis.h"

class ARpcTimerOnMillisTest
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcTimerOnMillisTest(QObject *parent=0);
	void testSingleTimer();
	void testRepeatedTimer();
	void testGetByIndex();

private:
	void testTimer(bool rep);
};

#endif // ARPCTIMERONMILLISTEST_H
