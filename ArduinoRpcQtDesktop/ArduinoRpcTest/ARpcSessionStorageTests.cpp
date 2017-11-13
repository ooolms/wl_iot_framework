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

#include "ARpcSessionStorageTests.h"
#include "RemoveDirRecusive.h"
#include "ARpcLocalStorage/ARpcSessionStorage.h"
#include "ARpcBase/ARpcSingleSensorValue.h"
#include "ARpcBase/ARpcPacketSensorValue.h"
#include "ARpcBase/ARpcTextSensorValue.h"
#include <QDateTime>

static const QString storPath=QString("/tmp/ARpcTestStorage");
static const QVector<quint32> sizesWithTime=
	QVector<quint32>()<<sizeof(qint64)<<sizeof(double)<<sizeof(double)<<sizeof(double);
static const QVector<quint32> sizesWithoutTime=
	QVector<quint32>()<<sizeof(double)<<sizeof(double)<<sizeof(double);
static const QVector<double> testData1=QVector<double>()<<12.0<<621.124<<2845.125626;
static const QVector<double> testData2=QVector<double>()<<156.6<<124.1<<0.1;
static const ARpcSensor sensorNT={"test_sensor",ARpcSensor::SINGLE,{{"dims","3"}}};
static const ARpcSensor sensorLT={"test_sensor",ARpcSensor::SINGLE_LT,{{"dims","3"}}};
static const ARpcSensor sensorGT={"test_sensor",ARpcSensor::SINGLE_GT,{{"dims","3"}}};
static const QUuid deviceId=QUuid("{9e693b9e-a6ef-4260-a5dd-0e1812fdf514}");
static const QString deviceName="test_device";

//CRIT test muliple sessions

ARpcSessionStorageTests::ARpcSessionStorageTests(QObject *parent)
	:QtUnitTestSet("ARpcSessionStorageTests",parent)
{
	addTest((TestFunction)&ARpcSessionStorageTests::testStorageSingleDontTouchTime,
		"Test storage for SINGLE values with time policy DONT_TOUCH");
	addTest((TestFunction)&ARpcSessionStorageTests::testStorageSingleLTDontTouchTime,
		"Test storage for SINGLE_LT values with time policy DONT_TOUCH");
}

void ARpcSessionStorageTests::testStorageSingleDontTouchTime()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcISensorStorage::preCreate(
		storPath,ARpcISensorStorage::AUTO_SESSIONS,sensorNT,deviceId,deviceName,ARpcISensorStorage::DONT_TOUCH);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::AUTO_SESSIONS);
	VERIFY(iStorage->sensor()==sensorNT);
	ARpcSessionStorage *storage=(ARpcSessionStorage*)iStorage;
	VERIFY(storage->createAsChainedBlocksDb());
	VERIFY(storage->effectiveValuesType()==ARpcSensor::SINGLE);

	//create session
	QUuid sId;
	VERIFY(storage->createSession("Session1",sId));
	VERIFY(storage->openMainWriteSession(sId))

	//test read/write
	ARpcSingleSensorValue sValNT(3);
	sValNT.fromData(testData1);
	VERIFY(storage->writeSensorValue(&sValNT));
	VERIFY(storage->valuesCount(storage->getMainWriteSessionId())==1);
	ARpcSingleSensorValue *sValNT2=(ARpcSingleSensorValue*)storage->valueAt(storage->getMainWriteSessionId(),0);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==ARpcSensor::SINGLE)
	VERIFY(sValNT2->dims()==3);
	VERIFY(sValNT2->values()==testData1);
	delete sValNT2;

	//test open existing
	delete storage;
	iStorage=ARpcISensorStorage::preOpen(storPath);
	VERIFY(iStorage);
	VERIFY(iStorage->open());
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::AUTO_SESSIONS);
	VERIFY(iStorage->sensor()==sensorNT);
	VERIFY(iStorage->effectiveValuesType()==ARpcSensor::SINGLE);
	storage=(ARpcSessionStorage*)iStorage;

	//test open session
	QList<QUuid> ids;
	QStringList titles;
	VERIFY(storage->listSessions(ids,titles));
	VERIFY(ids.count()==1&&titles.count()==1);
	VERIFY(ids[0]==sId&&titles[0]=="Session1");
	VERIFY(storage->openMainWriteSession(sId));

	//test read value
	VERIFY(storage->valuesCount(storage->getMainWriteSessionId())==1);
	sValNT2=(ARpcSingleSensorValue*)storage->valueAt(storage->getMainWriteSessionId(),0);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==ARpcSensor::SINGLE)
	VERIFY(sValNT2->dims()==3);
	VERIFY(sValNT2->values()==testData1);
	delete sValNT2;
}

void ARpcSessionStorageTests::testStorageSingleLTDontTouchTime()
{
	//continuous storage replaces LT by GT in any case
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcISensorStorage::preCreate(
		storPath,ARpcISensorStorage::MANUAL_SESSIONS,sensorLT,deviceId,deviceName,ARpcISensorStorage::DONT_TOUCH);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::MANUAL_SESSIONS);
	VERIFY(iStorage->sensor()==sensorLT);
	ARpcSessionStorage *storage=(ARpcSessionStorage*)iStorage;
	VERIFY(storage->createAsFixedBlocksDb(ARpcSingleSensorValue(3,true)));
	VERIFY(storage->effectiveValuesType()==ARpcSensor::SINGLE_LT);

	//create session
	QUuid sId;
	VERIFY(storage->createSession("Session1",sId));
	VERIFY(storage->openMainWriteSession(sId))

	//test read/write
	ARpcSingleSensorValue sValLT(3,true);
	sValLT.fromData(testData1);
	sValLT.setTime(10);
	VERIFY(storage->writeSensorValue(&sValLT));
	VERIFY(storage->valuesCount(storage->getMainWriteSessionId())==1);
	ARpcSingleSensorValue *sValLT2=(ARpcSingleSensorValue*)storage->valueAt(storage->getMainWriteSessionId(),0);
	VERIFY(sValLT2);
	VERIFY(sValLT2->type()==ARpcSensor::SINGLE_LT);
	VERIFY(sValLT2->dims()==3);
	VERIFY(sValLT2->values()==testData1);
	VERIFY(sValLT2->time()==sValLT.time())
	delete sValLT2;

	//test open existing
	delete storage;
	iStorage=ARpcISensorStorage::preOpen(storPath);
	VERIFY(iStorage);
	VERIFY(iStorage->open());
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::MANUAL_SESSIONS);
	VERIFY(iStorage->sensor()==sensorLT);
	VERIFY(iStorage->effectiveValuesType()==ARpcSensor::SINGLE_LT);
	storage=(ARpcSessionStorage*)iStorage;

	//test open session
	QList<QUuid> ids;
	QStringList titles;
	VERIFY(storage->listSessions(ids,titles));
	VERIFY(ids.count()==1&&titles.count()==1);
	VERIFY(ids[0]==sId&&titles[0]=="Session1");
	VERIFY(storage->openMainWriteSession(sId));

	//test read value
	VERIFY(storage->valuesCount(storage->getMainWriteSessionId())==1);
	sValLT2=(ARpcSingleSensorValue*)storage->valueAt(storage->getMainWriteSessionId(),0);
	VERIFY(sValLT2);
	VERIFY(sValLT2->type()==ARpcSensor::SINGLE_LT);
	VERIFY(sValLT2->dims()==3);
	VERIFY(sValLT2->values()==testData1);
	VERIFY(sValLT2->time()==sValLT.time());
	delete sValLT2;
}

bool ARpcSessionStorageTests::init()
{
	QDir dir(storPath);
	dir.mkpath(dir.path());
	return true;
}

void ARpcSessionStorageTests::cleanup()
{
	RemoveDirRecusive::rmDirRec(QDir(storPath));
}
