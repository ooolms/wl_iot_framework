#ifndef ARPCTTYWATCHERTESTS_H
#define ARPCTTYWATCHERTESTS_H

#include "QtUnitTestSet.h"

class ARpcTtyWatcherTests
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcTtyWatcherTests(QObject *parent=0);
	void testConnectionOnTheFly();
	void testStartupConnection();
};

#endif // ARPCTTYWATCHERTESTS_H
