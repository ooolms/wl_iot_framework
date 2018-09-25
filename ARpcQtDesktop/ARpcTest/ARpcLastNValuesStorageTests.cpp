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

#include "ARpcLastNValuesStorageTests.h"
#include "RemoveDirRecusive.h"
#include "ARpcStorages/ARpcLastNValuesStorage.h"
#include "TestData.h"
#include <QDateTime>

ARpcLastNValuesStorageTests::ARpcLastNValuesStorageTests(QObject *parent)
	:QtUnitTestSet("ARpcLastNValuesStorageTests",parent)
{
	addTest((TestFunction)&ARpcLastNValuesStorageTests::testStorageSingleDontTouchTime,
		"Test storage for single values");
	addTest((TestFunction)&ARpcLastNValuesStorageTests::testStorageSingleGTDontTouchTime,
		"Test storage for single values with global timestamp");
}

void ARpcLastNValuesStorageTests::testStorageSingleDontTouchTime()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcFSSensorStorageHelper::preCreate(
		storPath,deviceId,deviceName,singleNT,ARpcISensorStorage::LAST_N_VALUES,ARpcISensorStorage::DONT_TOUCH);
	VERIFY(iStorage->storeMode()==ARpcISensorStorage::LAST_N_VALUES);
	VERIFY(iStorage->sensor()==singleNT);
	ARpcLastNValuesStorage *storage=(ARpcLastNValuesStorage*)iStorage;
	VERIFY(storage->create(3));
	VERIFY(storage->storedValuesType()==singleNT.type);

	//test read/write
	ARpcSensorValueF32 sValNT(singleNT.type);
	sValNT.parseBinary(singleData1Binary);
	VERIFY(storage->writeSensorValue(&sValNT));
	VERIFY(storage->valuesCount()==3);
	ARpcSensorValue *sValNT2=storage->valueAt(storage->valuesCount()-1);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==singleNT.type)
	VERIFY(((ARpcSensorValueF32*)sValNT2)->getSample()==singleData1);
	delete sValNT2;

	//write second value
	sValNT.parseBinary(singleData2Binary);
	VERIFY(storage->writeSensorValue(&sValNT));
	VERIFY(storage->valuesCount()==3);
	sValNT2=storage->valueAt(storage->valuesCount()-1);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==singleNT.type)
	VERIFY(((ARpcSensorValueF32*)sValNT2)->getSample()==singleData2);
	delete sValNT2;
	sValNT2=storage->valueAt(storage->valuesCount()-2);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==singleNT.type)
	VERIFY(((ARpcSensorValueF32*)sValNT2)->getSample()==singleData1);
	delete sValNT2;

	//test open existing
	delete storage;
	iStorage=ARpcFSSensorStorageHelper::preOpen(storPath);
	VERIFY(iStorage);
	VERIFY(iStorage->open());
	VERIFY(iStorage->storeMode()==ARpcISensorStorage::LAST_N_VALUES);
	VERIFY(iStorage->sensor()==singleNT);
	VERIFY(iStorage->storedValuesType()==singleNT.type);
	storage=(ARpcLastNValuesStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==3);
	sValNT2=storage->valueAt(storage->valuesCount()-1);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==singleNT.type)
	VERIFY(((ARpcSensorValueF32*)sValNT2)->getSample()==singleData2);
	delete sValNT2;
	sValNT2=storage->valueAt(storage->valuesCount()-2);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==singleNT.type)
	VERIFY(((ARpcSensorValueF32*)sValNT2)->getSample()==singleData1);
	delete sValNT2;
}

void ARpcLastNValuesStorageTests::testStorageSingleGTDontTouchTime()
{
	//continuous storage replaces LT by GT in any case
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcFSSensorStorageHelper::preCreate(
		storPath,deviceId,deviceName,singleGT,ARpcISensorStorage::LAST_N_VALUES,ARpcISensorStorage::DONT_TOUCH);
	VERIFY(iStorage->storeMode()==ARpcISensorStorage::LAST_N_VALUES);
	VERIFY(iStorage->sensor()==singleGT);
	ARpcLastNValuesStorage *storage=(ARpcLastNValuesStorage*)iStorage;
	VERIFY(storage->create(1));
	VERIFY(storage->storedValuesType()==singleGT.type);

	//test read/write
	ARpcSensorValueF32 sValGT(singleGT.type);
	sValGT.parseMsgArgs(singleData1MsgArgsWithTs);
	VERIFY(storage->writeSensorValue(&sValGT));
	VERIFY(storage->valuesCount()==1);
	ARpcSensorValue *sValGT2=storage->valueAt(storage->valuesCount()-1);
	VERIFY(sValGT2);
	VERIFY(sValGT2->type()==singleGT.type);
	VERIFY(((ARpcSensorValueF32*)sValGT2)->getSample()==singleData1);
	VERIFY(sValGT2->time()==someTimestamp)
	delete sValGT2;

	//write second value
	sValGT.parseMsgArgs(singleData2MsgArgsWithTs);
	VERIFY(storage->writeSensorValue(&sValGT));
	VERIFY(storage->valuesCount()==1);
	sValGT2=storage->valueAt(0);
	VERIFY(sValGT2);
	VERIFY(sValGT2->type()==singleGT.type);
	VERIFY(((ARpcSensorValueF32*)sValGT2)->getSample()==singleData2);
	VERIFY(sValGT2->time()==someTimestamp)
	delete sValGT2;

	//test open existing
	delete storage;
	iStorage=ARpcFSSensorStorageHelper::preOpen(storPath);
	VERIFY(iStorage);
	VERIFY(iStorage->open());
	VERIFY(iStorage->storeMode()==ARpcISensorStorage::LAST_N_VALUES);
	VERIFY(iStorage->sensor()==singleGT);
	VERIFY(iStorage->storedValuesType()==singleGT.type);
	storage=(ARpcLastNValuesStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==1);
	sValGT2=storage->valueAt(0);
	VERIFY(sValGT2);
	VERIFY(sValGT2->type()==singleGT.type);
	VERIFY(((ARpcSensorValueF32*)sValGT2)->getSample()==singleData2);
	VERIFY(sValGT2->time()==someTimestamp)
	delete sValGT2;
}

bool ARpcLastNValuesStorageTests::init()
{
	QDir dir(storPath);
	if(!dir.exists())dir.mkpath(dir.absolutePath());
	RemoveDirRecusive::rmDirContentsRec(dir);
	return true;
}

void ARpcLastNValuesStorageTests::cleanup()
{
	QDir dir(storPath);
	RemoveDirRecusive::rmDirRec(dir);
}
