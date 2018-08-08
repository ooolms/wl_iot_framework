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
#include "TestData.h"
#include <QDateTime>

ARpcDBDriverHelpersTests::ARpcDBDriverHelpersTests(QObject *parent)
	:QtUnitTestSet("ARpcDBDriverHelpersTests",parent)
{
	addTest((TestFunction)&ARpcDBDriverHelpersTests::testGetTime,"Test getTimeFromVal function");
	addTest((TestFunction)&ARpcDBDriverHelpersTests::testPackUnpackValues,"Test packing and upacking values functions");
	addTest((TestFunction)&ARpcDBDriverHelpersTests::testPackUnpackValuesWithAddingTimestamp,
		"Test packing and upacking values with adding global timestamp");
}

void ARpcDBDriverHelpersTests::testGetTime()
{
	int hasTime;
	qint64 timeStamp;
	qint64 currentDT=QDateTime::currentMSecsSinceEpoch();
	ARpcSensorDef::Type type;
	type.dim=3;
	type.dim=3;
	type.numType=ARpcSensorDef::F32;
	type.packType=ARpcSensorDef::SINGLE;
	type.tsType=ARpcSensorDef::NO_TIME;
	ARpcSensorValueF32 sNT(type);
	type.tsType=ARpcSensorDef::LOCAL_TIME;
	ARpcSensorValueF32 sLT(type);
	type.tsType=ARpcSensorDef::GLOBAL_TIME;
	ARpcSensorValueF32 sGT(type);
	sLT.setTime(1);
	sGT.setTime(2);

	ARpcDBDriverHelpers hlp(ARpcISensorStorage::DONT_TOUCH);
	hlp.getTimestampForVal(&sNT,hasTime,timeStamp);
	VERIFY(hasTime==0);
	hlp.getTimestampForVal(&sLT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp==sLT.time());
	hlp.getTimestampForVal(&sGT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp==sGT.time());

	hlp=ARpcDBDriverHelpers(ARpcISensorStorage::ADD_GT);
	hlp.getTimestampForVal(&sNT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp>=currentDT);
	hlp.getTimestampForVal(&sLT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp>=currentDT);
	hlp.getTimestampForVal(&sGT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp==sGT.time());

	hlp=ARpcDBDriverHelpers(ARpcISensorStorage::DROP_TIME);
	hlp.getTimestampForVal(&sNT,hasTime,timeStamp);
	VERIFY(hasTime==0);
	hlp.getTimestampForVal(&sLT,hasTime,timeStamp);
	VERIFY(hasTime==0);
	hlp.getTimestampForVal(&sGT,hasTime,timeStamp);
	VERIFY(hasTime==0);
}

void ARpcDBDriverHelpersTests::testPackUnpackValues()
{
	ARpcDBDriverHelpers helper(ARpcISensorStorage::DONT_TOUCH);

	ARpcSensorValueF32 valNT(singleNT.type);
	valNT.parseBinary(singleData1Binary);
	QByteArray data=helper.packSensorValue(&valNT);
	ARpcSensorValueF32 valNT2(singleNT.type);
	valNT2.parseBinary(data);
	VERIFY(valNT==valNT2)

	ARpcSensorValueF32 valGT(singleGT.type);
	valGT.parseMsgArgs(singleData1MsgArgsWithTs);
	data=helper.packSensorValue(&valGT);
	ARpcSensorValueF32 valGT2(singleGT.type);
	valGT2.parseBinary(data);
	VERIFY(valGT==valGT2)
}

void ARpcDBDriverHelpersTests::testPackUnpackValuesWithAddingTimestamp()
{
	ARpcDBDriverHelpers helper(ARpcISensorStorage::ADD_GT);
	qint64 currentDT=QDateTime::currentMSecsSinceEpoch();

	ARpcSensorValueF32 valNT(singleNT.type);
	valNT.parseBinary(singleData1Binary);
	QByteArray data=helper.packSensorValue(&valNT);
	ARpcSensorValueF32 valGT2(singleGT.type);
	valGT2.parseBinary(data);
	VERIFY(valGT2.time()>=currentDT)
	VERIFY(valNT.getSample()==valGT2.getSample())
}
