#ifndef ARPCSTARNETENDPOINTTEST_H
#define ARPCSTARNETENDPOINTTEST_H

#include "QtUnitTestSet.h"

class ARpcStarNetEndPointTest
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcStarNetEndPointTest(QObject *parent=0);
	void testEscapingInNet();
};

#endif // ARPCSTARNETENDPOINTTEST_H
