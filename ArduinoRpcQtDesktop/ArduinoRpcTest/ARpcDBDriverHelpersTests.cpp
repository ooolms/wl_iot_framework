/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "ARpcDBDriverHelpersTests.h"
#include "ARpcLocalStorage/ARpcDBDriverHelpers.h"
#include "ARpcBase/ARpcPacketSensorValue.h"
#include "ARpcBase/ARpcSingleSensorValue.h"
#include "ARpcBase/ARpcTextSensorValue.h"
#include <QDateTime>

ARpcDBDriverHelpersTests::ARpcDBDriverHelpersTests(QObject *parent)
	:QtUnitTestSet("ARpcDBDriverHelpersTests",parent)
{
	addTest((TestFunction)&ARpcDBDriverHelpersTests::testDetectIfHasTime,"Test detectIfHasTime function");
	addTest((TestFunction)&ARpcDBDriverHelpersTests::testGetTime,"Test getTimeFromVal function");
}

void ARpcDBDriverHelpersTests::testDetectIfHasTime()
{
	int hasTime;
	ARpcDBDriverHelpers hlp(ARpcISensorStorage::DONT_TOUCH);
	hlp.detectIfHasTime(ARpcSensor::SINGLE,hasTime);
	VERIFY(hasTime==0);
	hlp.detectIfHasTime(ARpcSensor::SINGLE_LT,hasTime);
	VERIFY(hasTime==1);
	hlp.detectIfHasTime(ARpcSensor::SINGLE_GT,hasTime);
	VERIFY(hasTime==1);
	hlp.detectIfHasTime(ARpcSensor::PACKET,hasTime);
	VERIFY(hasTime==0);
	hlp.detectIfHasTime(ARpcSensor::PACKET_LT,hasTime);
	VERIFY(hasTime==1);
	hlp.detectIfHasTime(ARpcSensor::PACKET_GT,hasTime);
	VERIFY(hasTime==1);
	hlp.detectIfHasTime(ARpcSensor::TEXT,hasTime);
	VERIFY(hasTime==1);

	hlp=ARpcDBDriverHelpers(ARpcISensorStorage::ADD_GT);
	hlp.detectIfHasTime(ARpcSensor::SINGLE,hasTime);
	VERIFY(hasTime==1);
	hlp.detectIfHasTime(ARpcSensor::SINGLE_LT,hasTime);
	VERIFY(hasTime==1);
	hlp.detectIfHasTime(ARpcSensor::SINGLE_GT,hasTime);
	VERIFY(hasTime==1);
	hlp.detectIfHasTime(ARpcSensor::PACKET,hasTime);
	VERIFY(hasTime==1);
	hlp.detectIfHasTime(ARpcSensor::PACKET_LT,hasTime);
	VERIFY(hasTime==1);
	hlp.detectIfHasTime(ARpcSensor::PACKET_GT,hasTime);
	VERIFY(hasTime==1);
	hlp.detectIfHasTime(ARpcSensor::TEXT,hasTime);
	VERIFY(hasTime==1);

	hlp=ARpcDBDriverHelpers(ARpcISensorStorage::DROP_TIME);
	hlp.detectIfHasTime(ARpcSensor::SINGLE,hasTime);
	VERIFY(hasTime==0);
	hlp.detectIfHasTime(ARpcSensor::SINGLE_LT,hasTime);
	VERIFY(hasTime==0);
	hlp.detectIfHasTime(ARpcSensor::SINGLE_GT,hasTime);
	VERIFY(hasTime==0);
	hlp.detectIfHasTime(ARpcSensor::PACKET,hasTime);
	VERIFY(hasTime==0);
	hlp.detectIfHasTime(ARpcSensor::PACKET_LT,hasTime);
	VERIFY(hasTime==0);
	hlp.detectIfHasTime(ARpcSensor::PACKET_GT,hasTime);
	VERIFY(hasTime==0);
	hlp.detectIfHasTime(ARpcSensor::TEXT,hasTime);
	VERIFY(hasTime==1);
}

void ARpcDBDriverHelpersTests::testGetTime()
{
	int hasTime;
	qint64 timeStamp;
	qint64 currentDT=QDateTime::currentMSecsSinceEpoch();
	ARpcSingleSensorValue sNT(3),sLT(3,true),sGT(3,false);
	sLT.setTime(1);
	sGT.setTime(2);
	ARpcPacketSensorValue pNT(3),pLT(3,true),pGT(3,false);
	pLT.setTime(10);
	pGT.setTime(20);
	ARpcTextSensorValue t;
	t.setTime(3);

	ARpcDBDriverHelpers hlp(ARpcISensorStorage::DONT_TOUCH);
	hlp.getTimeFromVal(&sNT,hasTime,timeStamp);
	VERIFY(hasTime==0);
	hlp.getTimeFromVal(&sLT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp==sLT.time());
	hlp.getTimeFromVal(&sGT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp==sGT.time());
	hlp.getTimeFromVal(&pNT,hasTime,timeStamp);
	VERIFY(hasTime==0);
	hlp.getTimeFromVal(&pLT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp==pLT.time());
	hlp.getTimeFromVal(&pGT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp==pGT.time());
	hlp.getTimeFromVal(&t,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp==t.time());

	hlp=ARpcDBDriverHelpers(ARpcISensorStorage::ADD_GT);
	hlp.getTimeFromVal(&sNT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp>=currentDT);
	hlp.getTimeFromVal(&sLT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp>=currentDT);
	hlp.getTimeFromVal(&sGT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp==sGT.time());
	hlp.getTimeFromVal(&pNT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp>=currentDT);
	hlp.getTimeFromVal(&pLT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp>=currentDT);
	hlp.getTimeFromVal(&pGT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp==pGT.time());
	hlp.getTimeFromVal(&t,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp==t.time());

	hlp=ARpcDBDriverHelpers(ARpcISensorStorage::DROP_TIME);
	hlp.getTimeFromVal(&sNT,hasTime,timeStamp);
	VERIFY(hasTime==0);
	hlp.getTimeFromVal(&sLT,hasTime,timeStamp);
	VERIFY(hasTime==0);
	hlp.getTimeFromVal(&sGT,hasTime,timeStamp);
	VERIFY(hasTime==0);
	hlp.getTimeFromVal(&pNT,hasTime,timeStamp);
	VERIFY(hasTime==0);
	hlp.getTimeFromVal(&pLT,hasTime,timeStamp);
	VERIFY(hasTime==0);
	hlp.getTimeFromVal(&pGT,hasTime,timeStamp);
	VERIFY(hasTime==0);
	hlp.getTimeFromVal(&t,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp==t.time());
}
