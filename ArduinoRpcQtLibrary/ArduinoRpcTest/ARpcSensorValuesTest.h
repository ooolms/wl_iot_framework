#ifndef ARPCSENSORVALUESTEST_H
#define ARPCSENSORVALUESTEST_H

#include "QtUnitTestSet.h"

class ARpcSensorValuesTest
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcSensorValuesTest(QObject *parent=0);
	void testSingleValue();
	void testTextValue();
	void testPacketValue();
};

#endif // ARPCSENSORVALUESTEST_H
