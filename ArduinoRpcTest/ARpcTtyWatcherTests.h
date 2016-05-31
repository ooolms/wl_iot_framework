#ifndef ARPCTTYWATCHERTESTS_H
#define ARPCTTYWATCHERTESTS_H

#include "QtUnitTestSet.h"
#include "ARpcBase/ARpcConfig.h"

class ARpcTtyWatcherTests
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcTtyWatcherTests(QObject *parent=0);
	void testConnectionOnTheFly();
	void testStartupConnection();
	void testCallBreakWhenDevDisconnected();

private:
	ARpcConfig cfg;
};

#endif // ARPCTTYWATCHERTESTS_H
