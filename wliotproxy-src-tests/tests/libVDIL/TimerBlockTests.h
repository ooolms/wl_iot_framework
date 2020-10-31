#ifndef TIMERBLOCKTESTS_H
#define TIMERBLOCKTESTS_H

#include "QtUnitTestSet.h"

class TimerBlockTests
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit TimerBlockTests(QObject *parent=nullptr);
	void testCalcNextTimeoutSingle();
	void testCalcNextTimeoutRegular();
	void testCalcNextTimeoutMonthly();
	void testCalcNextTimeoutYearly();
};

#endif // TIMERBLOCKTESTS_H
