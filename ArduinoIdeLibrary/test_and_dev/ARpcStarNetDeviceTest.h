#ifndef ARPCSTARNETDEVICETEST_H
#define ARPCSTARNETDEVICETEST_H

#include "QtUnitTestSet.h"

class ARpcStarNetDeviceTest
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcStarNetDeviceTest(QObject *parent=nullptr);
	void testDirectMsg();
	void testUndirectMsg();
};

#endif // ARPCSTARNETDEVICETEST_H
