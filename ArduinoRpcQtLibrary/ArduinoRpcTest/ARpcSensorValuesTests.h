#ifndef ARPCSENSORVALUESTESTS_H
#define ARPCSENSORVALUESTESTS_H

#include "QtUnitTestSet.h"

class ARpcSensorValuesTests
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcSensorValuesTests(QObject *parent=0);
	void testSingleValue();
	void testTextValue();
	void testPacketValue();
};

#endif // ARPCSENSORVALUESTESTS_H
