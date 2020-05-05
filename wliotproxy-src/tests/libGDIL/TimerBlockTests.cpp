#include "TimerBlockTests.h"
#include "GDIL/core/TimerBlock.h"

TimerBlockTests::TimerBlockTests(QObject *parent)
	:QtUnit::QtUnitTestSet("TimerBlockTests",parent)
{
	addTest((TestFunction)&TimerBlockTests::testCalcNextTimeoutSingle,
		"test calculating next timeout for a single shot timer");
	addTest((TestFunction)&TimerBlockTests::testCalcNextTimeoutRegular,
		"test calculating next timeout for a regular (each X secs, mins, hours, days) timer");
	addTest((TestFunction)&TimerBlockTests::testCalcNextTimeoutMonthly,
		"test calculating next timeout for a monthly timer");
	addTest((TestFunction)&TimerBlockTests::testCalcNextTimeoutYearly,
		"test calculating next timeout for a yearly timer");
}

void TimerBlockTests::testCalcNextTimeoutSingle()
{
	QDateTime startTime=QDateTime(QDate(2010,5,1),QTime(12,0));
	QDateTime before1Time=QDateTime(QDate(2010,4,1),QTime(12,0));
	QDateTime before2Time=QDateTime(QDate(2010,5,1),QTime(11,0));
	QDateTime after1Time=QDateTime(QDate(2010,5,1),QTime(13,0));
	QDateTime after2Time=QDateTime(QDate(2010,6,1),QTime(12,0));

	TimerBlock b(0);
	b.setConfig({startTime,TimerBlock::SINGLE,0},false);

	VERIFY(b.nextTimeout(before1Time)==startTime)
	VERIFY(b.nextTimeout(before1Time)==startTime)
	VERIFY(!b.nextTimeout(after1Time).isValid())
	VERIFY(!b.nextTimeout(after2Time).isValid())
}

void TimerBlockTests::testCalcNextTimeoutRegular()
{
	QDateTime startTime=QDateTime(QDate(2010,5,1),QTime(12,0));
	TimerBlock b(0);

	b.setConfig({startTime,TimerBlock::REGULAR_SEC,3},false);
	QDateTime d=b.nextTimeout(startTime.addSecs(1));
	VERIFY(startTime.secsTo(d)==3)

	b.setConfig({startTime,TimerBlock::REGULAR_MIN,5},false);
	d=b.nextTimeout(startTime.addSecs(1));
	VERIFY(startTime.secsTo(d)==(5*60))

	b.setConfig({startTime,TimerBlock::REGULAR_HOUR,2},false);
	d=b.nextTimeout(startTime.addSecs(1));
	VERIFY(startTime.secsTo(d)==(2*3600))
	d=b.nextTimeout(startTime.addSecs(3601));
	VERIFY(startTime.secsTo(d)==(2*3600))
	d=b.nextTimeout(startTime.addSecs(7201));
	VERIFY(startTime.secsTo(d)==(4*3600))

	b.setConfig({startTime,TimerBlock::REGULAR_DAY,3},false);
	d=b.nextTimeout(startTime.addSecs(1).addDays(2));
	VERIFY(startTime.secsTo(d)==(3*24*3600))
	d=b.nextTimeout(startTime.addDays(3));
	VERIFY(startTime.secsTo(d)==(6*24*3600))
}

void TimerBlockTests::testCalcNextTimeoutMonthly()
{
	QDateTime startTime=QDateTime(QDate(2010,5,1),QTime(12,0));
	TimerBlock b(0);

	b.setConfig({startTime,TimerBlock::MONTH,0},false);
	QDateTime d=b.nextTimeout(startTime.addSecs(1));
	VERIFY(d.time()==startTime.time())
	VERIFY(d.date()==QDate(2010,6,1))

	startTime=QDateTime(QDate(2010,3,31),QTime(12,0));
	b.setConfig({startTime,TimerBlock::MONTH,0},false);
	d=b.nextTimeout(startTime.addSecs(1));
	VERIFY(d.time()==startTime.time())
	VERIFY(d.date()==QDate(2010,4,30))

	startTime=QDateTime(QDate(2010,1,31),QTime(12,0));
	b.setConfig({startTime,TimerBlock::MONTH,0},false);
	d=b.nextTimeout(QDateTime(QDate(2010,6,1),QTime(12,0)));
	VERIFY(d.time()==startTime.time())
	VERIFY(d.date()==QDate(2010,6,30))
}

void TimerBlockTests::testCalcNextTimeoutYearly()
{
	QDateTime startTime=QDateTime(QDate(2010,5,1),QTime(12,0));
	TimerBlock b(0);

	b.setConfig({startTime,TimerBlock::YEAR,0},false);
	QDateTime d=b.nextTimeout(startTime.addSecs(1));
	VERIFY(d.time()==startTime.time())
	VERIFY(d.date()==QDate(2011,5,1))

	startTime=QDateTime(QDate(2012,2,29),QTime(12,0));
	b.setConfig({startTime,TimerBlock::YEAR,0},false);
	d=b.nextTimeout(startTime.addSecs(1));
	VERIFY(d.time()==startTime.time())
	VERIFY(d.date()==QDate(2013,2,28))
}
