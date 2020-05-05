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

#include "LastNValuesStorageTests.h"
#include "RemoveDirRecusive.h"
#include "wliot/storages/LastNValuesStorage.h"
#include "FSSensorStorageHelper.h"
#include "TestData.h"
#include <QDateTime>

LastNValuesStorageTests::LastNValuesStorageTests(QObject *parent)
	:QtUnitTestSet("LastNValuesStorageTests",parent)
{
	addTest((TestFunction)&LastNValuesStorageTests::testStorageSingleDontTouchTime,
		"Test storage for single values");
	addTest((TestFunction)&LastNValuesStorageTests::testStorageSingleGTDontTouchTime,
		"Test storage for single values with global timestamp");
}

void LastNValuesStorageTests::testStorageSingleDontTouchTime()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ISensorStorage *iStorage=FSSensorStorageHelper::preCreate(
		storPath,deviceId,deviceName,singleNT,ISensorStorage::LAST_N_VALUES,ISensorStorage::DONT_TOUCH);
	VERIFY(iStorage->storeMode()==ISensorStorage::LAST_N_VALUES);
	VERIFY(iStorage->sensor()==singleNT);
	LastNValuesStorage *storage=(LastNValuesStorage*)iStorage;
	VERIFY(storage->create(3));
	VERIFY(storage->storedValuesType()==singleNT.type);

	//test read/write
	SensorValueF32 sValNT(singleNT.type);
	sValNT.parseBinary(singleData1Binary);
	VERIFY(storage->writeSensorValue(&sValNT));
	VERIFY(storage->valuesCount()==3);
	SensorValue *sValNT2=storage->valueAt(storage->valuesCount()-1);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==singleNT.type)
	VERIFY(((SensorValueF32*)sValNT2)->getSample(0)==singleData1);
	delete sValNT2;

	//write second value
	sValNT.parseBinary(singleData2Binary);
	VERIFY(storage->writeSensorValue(&sValNT));
	VERIFY(storage->valuesCount()==3);
	sValNT2=storage->valueAt(storage->valuesCount()-1);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==singleNT.type)
	VERIFY(((SensorValueF32*)sValNT2)->getSample(0)==singleData2);
	delete sValNT2;
	sValNT2=storage->valueAt(storage->valuesCount()-2);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==singleNT.type)
	VERIFY(((SensorValueF32*)sValNT2)->getSample(0)==singleData1);
	delete sValNT2;

	//test open existing
	delete storage;
	iStorage=FSSensorStorageHelper::preOpen(storPath);
	VERIFY(iStorage);
	VERIFY(iStorage->open());
	VERIFY(iStorage->storeMode()==ISensorStorage::LAST_N_VALUES);
	VERIFY(iStorage->sensor()==singleNT);
	VERIFY(iStorage->storedValuesType()==singleNT.type);
	storage=(LastNValuesStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==3);
	sValNT2=storage->valueAt(storage->valuesCount()-1);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==singleNT.type)
	VERIFY(((SensorValueF32*)sValNT2)->getSample(0)==singleData2);
	delete sValNT2;
	sValNT2=storage->valueAt(storage->valuesCount()-2);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==singleNT.type)
	VERIFY(((SensorValueF32*)sValNT2)->getSample(0)==singleData1);
	delete sValNT2;
}

void LastNValuesStorageTests::testStorageSingleGTDontTouchTime()
{
	//continuous storage replaces LT by GT in any case
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ISensorStorage *iStorage=FSSensorStorageHelper::preCreate(
		storPath,deviceId,deviceName,singleGT,ISensorStorage::LAST_N_VALUES,ISensorStorage::DONT_TOUCH);
	VERIFY(iStorage->storeMode()==ISensorStorage::LAST_N_VALUES);
	VERIFY(iStorage->sensor()==singleGT);
	LastNValuesStorage *storage=(LastNValuesStorage*)iStorage;
	VERIFY(storage->create(1));
	VERIFY(storage->storedValuesType()==singleGT.type);

	//test read/write
	SensorValueF32 sValGT(singleGT.type);
	sValGT.parseMsgArgs(singleData1MsgArgsWithTs);
	VERIFY(storage->writeSensorValue(&sValGT));
	VERIFY(storage->valuesCount()==1);
	SensorValue *sValGT2=storage->valueAt(storage->valuesCount()-1);
	VERIFY(sValGT2);
	VERIFY(sValGT2->type()==singleGT.type);
	VERIFY(((SensorValueF32*)sValGT2)->getSample(0)==singleData1);
	VERIFY(sValGT2->time()==someTimestamp)
	delete sValGT2;

	//write second value
	sValGT.parseMsgArgs(singleData2MsgArgsWithTs);
	VERIFY(storage->writeSensorValue(&sValGT));
	VERIFY(storage->valuesCount()==1);
	sValGT2=storage->valueAt(0);
	VERIFY(sValGT2);
	VERIFY(sValGT2->type()==singleGT.type);
	VERIFY(((SensorValueF32*)sValGT2)->getSample(0)==singleData2);
	VERIFY(sValGT2->time()==someTimestamp)
	delete sValGT2;

	//test open existing
	delete storage;
	iStorage=FSSensorStorageHelper::preOpen(storPath);
	VERIFY(iStorage);
	VERIFY(iStorage->open());
	VERIFY(iStorage->storeMode()==ISensorStorage::LAST_N_VALUES);
	VERIFY(iStorage->sensor()==singleGT);
	VERIFY(iStorage->storedValuesType()==singleGT.type);
	storage=(LastNValuesStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==1);
	sValGT2=storage->valueAt(0);
	VERIFY(sValGT2);
	VERIFY(sValGT2->type()==singleGT.type);
	VERIFY(((SensorValueF32*)sValGT2)->getSample(0)==singleData2);
	VERIFY(sValGT2->time()==someTimestamp)
	delete sValGT2;
}

bool LastNValuesStorageTests::init()
{
	QDir dir(storPath);
	if(!dir.exists())dir.mkpath(dir.absolutePath());
	RemoveDirRecusive::rmDirContentsRec(dir);
	return true;
}

void LastNValuesStorageTests::cleanup()
{
	QDir dir(storPath);
	RemoveDirRecusive::rmDirRec(dir);
}
