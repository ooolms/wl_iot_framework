#include "ARpcSensorValuesTest.h"

ARpcSensorValuesTest::ARpcSensorValuesTest(QObject *parent)
	:QtUnitTestSet("ARpcSensorValuesTest",parent)
{
	addTest((TestFunction)&ARpcSensorValuesTest::testSingleValue,"Test parsing single measurement");
	addTest((TestFunction)&ARpcSensorValuesTest::testPacketValue,"Test parsing packet of measurements");
	addTest((TestFunction)&ARpcSensorValuesTest::testTextValue,"Test parsing text");
}

