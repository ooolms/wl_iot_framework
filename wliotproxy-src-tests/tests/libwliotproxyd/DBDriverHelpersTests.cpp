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

#include "DBDriverHelpersTests.h"
#include "DBDriverHelpers.h"
#include "TestData.h"
#include <QDateTime>

using namespace WLIOT;

DBDriverHelpersTests::DBDriverHelpersTests(QObject *parent)
	:QtUnitTestSet("DBDriverHelpersTests",parent)
{
	addTest((TestFunction)&DBDriverHelpersTests::testGetTime,"Test getTimeFromVal function");
	addTest((TestFunction)&DBDriverHelpersTests::testPackUnpackValues,"Test packing and upacking values functions");
	addTest((TestFunction)&DBDriverHelpersTests::testPackUnpackValuesWithAddingTimestamp,
		"Test packing and upacking values with adding global timestamp");
}

void DBDriverHelpersTests::testGetTime()
{
	int hasTime;
	qint64 timeStamp;
	qint64 currentDT=QDateTime::currentMSecsSinceEpoch();
	SensorDef::Type type;
	type.dim=3;
	type.dim=3;
	type.numType=SensorDef::F32;
	type.packType=SensorDef::SINGLE;
	type.tsType=SensorDef::NO_TIME;
	SensorValueF32 sNT(type);
	type.tsType=SensorDef::LOCAL_TIME;
	SensorValueF32 sLT(type);
	type.tsType=SensorDef::GLOBAL_TIME;
	SensorValueF32 sGT(type);
	sLT.setTime(1);
	sGT.setTime(2);

	DBDriverHelpers hlp(ISensorStorage::DONT_TOUCH);
	hlp.getTimestampForVal(&sNT,hasTime,timeStamp);
	VERIFY(hasTime==0)
	hlp.getTimestampForVal(&sLT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp==sLT.time())
	hlp.getTimestampForVal(&sGT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp==sGT.time())

	hlp=DBDriverHelpers(ISensorStorage::ADD_GT);
	hlp.getTimestampForVal(&sNT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp>=currentDT)
	hlp.getTimestampForVal(&sLT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp>=currentDT)
	hlp.getTimestampForVal(&sGT,hasTime,timeStamp);
	VERIFY(hasTime==1&&timeStamp==sGT.time())

	hlp=DBDriverHelpers(ISensorStorage::DROP_TIME);
	hlp.getTimestampForVal(&sNT,hasTime,timeStamp);
	VERIFY(hasTime==0)
	hlp.getTimestampForVal(&sLT,hasTime,timeStamp);
	VERIFY(hasTime==0)
	hlp.getTimestampForVal(&sGT,hasTime,timeStamp);
	VERIFY(hasTime==0)
}

void DBDriverHelpersTests::testPackUnpackValues()
{
	DBDriverHelpers helper(ISensorStorage::DONT_TOUCH);

	SensorValueF32 valNT(singleNT.type);
	valNT.parseBinary(singleData1Binary);
	QByteArray data=helper.packSensorValue(&valNT);
	SensorValueF32 valNT2(singleNT.type);
	valNT2.parseBinary(data);
	VERIFY(valNT==valNT2)

	SensorValueF32 valGT(singleGT.type);
	valGT.parseMsgArgs(singleData1MsgArgsWithTs);
	data=helper.packSensorValue(&valGT);
	SensorValueF32 valGT2(singleGT.type);
	valGT2.parseBinary(data);
	VERIFY(valGT==valGT2)
}

void DBDriverHelpersTests::testPackUnpackValuesWithAddingTimestamp()
{
	DBDriverHelpers helper(ISensorStorage::ADD_GT);
	qint64 currentDT=QDateTime::currentMSecsSinceEpoch();

	SensorValueF32 valNT(singleNT.type);
	valNT.parseBinary(singleData1Binary);
	QByteArray data=helper.packSensorValue(&valNT);
	SensorValueF32 valGT2(singleGT.type);
	valGT2.parseBinary(data);
	VERIFY(valGT2.time()>=currentDT)
	VERIFY(valNT.getSample(0)==valGT2.getSample(0))
}
