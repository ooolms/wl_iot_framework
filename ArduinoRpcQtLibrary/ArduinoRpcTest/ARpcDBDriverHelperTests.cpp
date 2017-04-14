#include "ARpcDBDriverHelperTests.h"
#include "ARpcLocalStorage/ARpcDBDriverHelpers.h"
#include "ARpcBase/ARpcSingleSensorValue.h"
#include "ARpcBase/ARpcPacketSensorValue.h"
#include "ARpcBase/ARpcTextSensorValue.h"
#include <QDateTime>

ARpcDBDriverHelperTests::ARpcDBDriverHelperTests(QObject *parent)
	:QtUnitTestSet("ARpcDBDriverHelperTests",parent)
{
	addTest((TestFunction)&ARpcDBDriverHelperTests::testTimeFunctions,"Test time functions");
}

void ARpcDBDriverHelperTests::testTimeFunctions()
{
	int hasTime=0;
	qint64 tt=0;
	qint64 startTime=QDateTime::currentMSecsSinceEpoch();

	ARpcDBDriverHelpers hlp(ARpcISensorStorage::DONT_TOUCH);
	hlp.detectIfHasTime(ARpcSensor::SINGLE,hasTime);
	VERIFY(hasTime==0)
	hlp.detectIfHasTime(ARpcSensor::SINGLE_LT,hasTime);
	VERIFY(hasTime==1)
	hlp.detectIfHasTime(ARpcSensor::SINGLE_GT,hasTime);
	VERIFY(hasTime==1)
	hlp.detectIfHasTime(ARpcSensor::PACKET,hasTime);
	VERIFY(hasTime==0)
	hlp.detectIfHasTime(ARpcSensor::PACKET_LT,hasTime);
	VERIFY(hasTime==1)
	hlp.detectIfHasTime(ARpcSensor::PACKET_GT,hasTime);
	VERIFY(hasTime==1)
	hlp.detectIfHasTime(ARpcSensor::TEXT,hasTime);
	VERIFY(hasTime==1)

	hlp=ARpcDBDriverHelpers(ARpcISensorStorage::ADD_GT);
	hlp.detectIfHasTime(ARpcSensor::SINGLE,hasTime);
	VERIFY(hasTime==1)
	hlp.detectIfHasTime(ARpcSensor::SINGLE_LT,hasTime);
	VERIFY(hasTime==1)
	hlp.detectIfHasTime(ARpcSensor::SINGLE_GT,hasTime);
	VERIFY(hasTime==1)
	hlp.detectIfHasTime(ARpcSensor::PACKET,hasTime);
	VERIFY(hasTime==1)
	hlp.detectIfHasTime(ARpcSensor::PACKET_LT,hasTime);
	VERIFY(hasTime==1)
	hlp.detectIfHasTime(ARpcSensor::PACKET_GT,hasTime);
	VERIFY(hasTime==1)
	hlp.detectIfHasTime(ARpcSensor::TEXT,hasTime);
	VERIFY(hasTime==1)
	hlp=ARpcDBDriverHelpers(ARpcISensorStorage::DROP_TIME);
	hlp.detectIfHasTime(ARpcSensor::SINGLE,hasTime);
	VERIFY(hasTime==0)
	hlp.detectIfHasTime(ARpcSensor::SINGLE_LT,hasTime);
	VERIFY(hasTime==0)
	hlp.detectIfHasTime(ARpcSensor::SINGLE_GT,hasTime);
	VERIFY(hasTime==0)
	hlp.detectIfHasTime(ARpcSensor::PACKET,hasTime);
	VERIFY(hasTime==0)
	hlp.detectIfHasTime(ARpcSensor::PACKET_LT,hasTime);
	VERIFY(hasTime==0)
	hlp.detectIfHasTime(ARpcSensor::PACKET_GT,hasTime);
	VERIFY(hasTime==0)
	hlp.detectIfHasTime(ARpcSensor::TEXT,hasTime);
	VERIFY(hasTime==1)

	ARpcSingleSensorValue singleNT(1),singleLT(1,true),singleGT(1,false);
	ARpcPacketSensorValue packetNT(1),packetLT(1,true),packetGT(1,false);
	ARpcTextSensorValue text;
	singleNT.setTime(0);
	singleLT.setTime(1);
	singleGT.setTime(2);
	packetNT.setTime(0);
	packetLT.setTime(11);
	packetGT.setTime(22);
	text.setTime(222);

	hlp=ARpcDBDriverHelpers(ARpcISensorStorage::DONT_TOUCH);
	hlp.getTimeFromVal(&singleNT,hasTime,tt);
	VERIFY(hasTime==0)
	hlp.getTimeFromVal(&singleLT,hasTime,tt);
	VERIFY(hasTime==1&&tt==1)
	hlp.getTimeFromVal(&singleGT,hasTime,tt);
	VERIFY(hasTime==1&&tt==2)
	hlp.getTimeFromVal(&packetNT,hasTime,tt);
	VERIFY(hasTime==0)
	hlp.getTimeFromVal(&packetLT,hasTime,tt);
	VERIFY(hasTime==1&&tt==11)
	hlp.getTimeFromVal(&packetGT,hasTime,tt);
	VERIFY(hasTime==1&&tt==22)
	hlp.getTimeFromVal(&text,hasTime,tt);
	VERIFY(hasTime==1&&tt==222)

	hlp=ARpcDBDriverHelpers(ARpcISensorStorage::ADD_GT);
	hlp.getTimeFromVal(&singleNT,hasTime,tt);
	VERIFY(hasTime==1&&tt>=startTime)
	hlp.getTimeFromVal(&singleLT,hasTime,tt);
	VERIFY(hasTime==1&&tt>=startTime)
	hlp.getTimeFromVal(&singleGT,hasTime,tt);
	VERIFY(hasTime==1&&tt==2)
	hlp.getTimeFromVal(&packetNT,hasTime,tt);
	VERIFY(hasTime==1&&tt>=startTime)
	hlp.getTimeFromVal(&packetLT,hasTime,tt);
	VERIFY(hasTime==1&&tt>=startTime)
	hlp.getTimeFromVal(&packetGT,hasTime,tt);
	VERIFY(hasTime==1&&tt==22)
	hlp.getTimeFromVal(&text,hasTime,tt);
	VERIFY(hasTime==1&&tt==222)

	hlp=ARpcDBDriverHelpers(ARpcISensorStorage::DROP_TIME);
	hlp.getTimeFromVal(&singleNT,hasTime,tt);
	VERIFY(hasTime==0)
	hlp.getTimeFromVal(&singleLT,hasTime,tt);
	VERIFY(hasTime==0)
	hlp.getTimeFromVal(&singleGT,hasTime,tt);
	VERIFY(hasTime==0)
	hlp.getTimeFromVal(&packetNT,hasTime,tt);
	VERIFY(hasTime==0)
	hlp.getTimeFromVal(&packetLT,hasTime,tt);
	VERIFY(hasTime==0)
	hlp.getTimeFromVal(&packetGT,hasTime,tt);
	VERIFY(hasTime==0)
	hlp.getTimeFromVal(&text,hasTime,tt);
	VERIFY(hasTime==1&&tt==222)
}
