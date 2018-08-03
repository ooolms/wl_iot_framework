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
	hlp.detectIfHasTime(ARpcSensorDef::SINGLE,hasTime);
	VERIFY(hasTime==0);
	hlp.detectIfHasTime(ARpcSensorDef::SINGLE_LT,hasTime);
	VERIFY(hasTime==1);
	hlp.detectIfHasTime(ARpcSensorDef::SINGLE_GT,hasTime);
	VERIFY(hasTime==1);
	hlp.detectIfHasTime(ARpcSensorDef::PACKET,hasTime);
	VERIFY(hasTime==0);
	hlp.detectIfHasTime(ARpcSensorDef::PACKET_LT,hasTime);
	VERIFY(hasTime==1);
	hlp.detectIfHasTime(ARpcSensorDef::PACKET_GT,hasTime);
	VERIFY(hasTime==1);
	hlp.detectIfHasTime(ARpcSensorDef::TEXT,hasTime);
	VERIFY(hasTime==1);

	hlp=ARpcDBDriverHelpers(ARpcISensorStorage::ADD_GT);
	hlp.detectIfHasTime(ARpcSensorDef::SINGLE,hasTime);
	VERIFY(hasTime==1);
	hlp.detectIfHasTime(ARpcSensorDef::SINGLE_LT,hasTime);
	VERIFY(hasTime==1);
	hlp.detectIfHasTime(ARpcSensorDef::SINGLE_GT,hasTime);
	VERIFY(hasTime==1);
	hlp.detectIfHasTime(ARpcSensorDef::PACKET,hasTime);
	VERIFY(hasTime==1);
	hlp.detectIfHasTime(ARpcSensorDef::PACKET_LT,hasTime);
	VERIFY(hasTime==1);
	hlp.detectIfHasTime(ARpcSensorDef::PACKET_GT,hasTime);
	VERIFY(hasTime==1);
	hlp.detectIfHasTime(ARpcSensorDef::TEXT,hasTime);
	VERIFY(hasTime==1);

	hlp=ARpcDBDriverHelpers(ARpcISensorStorage::DROP_TIME);
	hlp.detectIfHasTime(ARpcSensorDef::SINGLE,hasTime);
	VERIFY(hasTime==0);
	hlp.detectIfHasTime(ARpcSensorDef::SINGLE_LT,hasTime);
	VERIFY(hasTime==0);
	hlp.detectIfHasTime(ARpcSensorDef::SINGLE_GT,hasTime);
	VERIFY(hasTime==0);
	hlp.detectIfHasTime(ARpcSensorDef::PACKET,hasTime);
	VERIFY(hasTime==0);
	hlp.detectIfHasTime(ARpcSensorDef::PACKET_LT,hasTime);
	VERIFY(hasTime==0);
	hlp.detectIfHasTime(ARpcSensorDef::PACKET_GT,hasTime);
	VERIFY(hasTime==0);
	hlp.detectIfHasTime(ARpcSensorDef::TEXT,hasTime);
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
	hlp.getTimestampForVal(&sNT,hasTime,timeStamp);
	VERIFY(hasTime==0);
	hlp.getTimestampForVal(&sLT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp==sLT.time());
	hlp.getTimestampForVal(&sGT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp==sGT.time());
	hlp.getTimestampForVal(&pNT,hasTime,timeStamp);
	VERIFY(hasTime==0);
	hlp.getTimestampForVal(&pLT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp==pLT.time());
	hlp.getTimestampForVal(&pGT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp==pGT.time());
	hlp.getTimestampForVal(&t,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp==t.time());

	hlp=ARpcDBDriverHelpers(ARpcISensorStorage::ADD_GT);
	hlp.getTimestampForVal(&sNT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp>=currentDT);
	hlp.getTimestampForVal(&sLT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp>=currentDT);
	hlp.getTimestampForVal(&sGT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp==sGT.time());
	hlp.getTimestampForVal(&pNT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp>=currentDT);
	hlp.getTimestampForVal(&pLT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp>=currentDT);
	hlp.getTimestampForVal(&pGT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp==pGT.time());
	hlp.getTimestampForVal(&t,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp==t.time());

	hlp=ARpcDBDriverHelpers(ARpcISensorStorage::DROP_TIME);
	hlp.getTimestampForVal(&sNT,hasTime,timeStamp);
	VERIFY(hasTime==0);
	hlp.getTimestampForVal(&sLT,hasTime,timeStamp);
	VERIFY(hasTime==0);
	hlp.getTimestampForVal(&sGT,hasTime,timeStamp);
	VERIFY(hasTime==0);
	hlp.getTimestampForVal(&pNT,hasTime,timeStamp);
	VERIFY(hasTime==0);
	hlp.getTimestampForVal(&pLT,hasTime,timeStamp);
	VERIFY(hasTime==0);
	hlp.getTimestampForVal(&pGT,hasTime,timeStamp);
	VERIFY(hasTime==0);
	hlp.getTimestampForVal(&t,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp==t.time());
}
