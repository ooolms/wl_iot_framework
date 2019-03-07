#ifndef ARPCDBDRIVERHELPERTESTS_H
#define ARPCDBDRIVERHELPERTESTS_H

#include "QtUnitTestSet.h"

class ARpcDBDriverHelperTests
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcDBDriverHelperTests(QObject *parent=0);
	void testTimeFunctions();
};

#endif // ARPCDBDRIVERHELPERTESTS_H
