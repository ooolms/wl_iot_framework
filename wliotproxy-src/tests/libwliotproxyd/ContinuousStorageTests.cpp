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

#include "ContinuousStorageTests.h"
#include "RemoveDirRecusive.h"
#include "wliot/storages/ContinuousStorage.h"
#include "FSSensorStorageHelper.h"
#include "TestData.h"
#include <QDateTime>

ContinuousStorageTests::ContinuousStorageTests(QObject *parent)
	:QtUnitTestSet("ContinuousStorageTests",parent)
{
	addTest((TestFunction)&ContinuousStorageTests::testStorageSingleDontTouchTime,
		"Test storage for single values with time policy DONT_TOUCH");
	addTest((TestFunction)&ContinuousStorageTests::testStorageSingleAddGT,
		"Test storage for single values with time policy ADD_GT");
	addTest((TestFunction)&ContinuousStorageTests::testStorageSingleLTDontTouchTime,
		"Test storage for single values with local timestamps with time policy DONT_TOUCH");
	addTest((TestFunction)&ContinuousStorageTests::testStorageSingleGTDropTime,
		"Test storage for single values with global timestamps with time policy DROP_TIME");
	addTest((TestFunction)&ContinuousStorageTests::testStoragePacketNTDontTouchTime,
		"Test storage for packet values with time policy DONT_TOUCH");
	addTest((TestFunction)&ContinuousStorageTests::testStoragePacketGTDontTouchTime,
		"Test storage for packet values with global timestamp with time policy DONT_TOUCH");
}

void ContinuousStorageTests::testStorageSingleDontTouchTime()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ISensorStorage *iStorage=FSSensorStorageHelper::preCreate(
		storPath,deviceId,deviceName,singleNT,ISensorStorage::CONTINUOUS,ISensorStorage::DONT_TOUCH);
	VERIFY(iStorage->storeMode()==ISensorStorage::CONTINUOUS)
	VERIFY(iStorage->sensor()==singleNT)
	ContinuousStorage *storage=(ContinuousStorage*)iStorage;
	VERIFY(storage->create())
	VERIFY(storage->storedValuesType()==singleNT.type)

	//test read/write
	SensorValueF32 sValNT(singleNT.type);
	sValNT.parseBinary(singleData1Binary);
	VERIFY(storage->writeSensorValue(&sValNT))
	VERIFY(storage->valuesCount()==1)
	SensorValue *sValNT2=storage->valueAt(0);
	VERIFY(sValNT2)
	VERIFY(sValNT2->type()==singleNT.type)
	VERIFY(((SensorValueF32*)sValNT2)->getSample(0)==singleData1)
	delete sValNT2;

	//test open existing
	delete storage;
	iStorage=FSSensorStorageHelper::preOpen(storPath);
	VERIFY(iStorage)
	VERIFY(iStorage->open())
	VERIFY(iStorage->storeMode()==ISensorStorage::CONTINUOUS)
	VERIFY(iStorage->sensor()==singleNT)
	VERIFY(iStorage->storedValuesType()==singleNT.type)
	storage=(ContinuousStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==1)
	sValNT2=storage->valueAt(0);
	VERIFY(sValNT2)
	VERIFY(sValNT2->type()==singleNT.type)
	VERIFY(((SensorValueF32*)sValNT2)->getSample(0)==singleData1)
	delete sValNT2;
}

void ContinuousStorageTests::testStorageSingleAddGT()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));
	qint64 currentDT=QDateTime::currentMSecsSinceEpoch();

	//test creation as fixed blocks storage
	ISensorStorage *iStorage=FSSensorStorageHelper::preCreate(
		storPath,deviceId,deviceName,singleNT,ISensorStorage::CONTINUOUS,ISensorStorage::ADD_GT);
	VERIFY(iStorage->storeMode()==ISensorStorage::CONTINUOUS)
	VERIFY(iStorage->sensor()==singleNT)
	ContinuousStorage *storage=(ContinuousStorage*)iStorage;
	VERIFY(storage->create())
	VERIFY(storage->storedValuesType()==singleGT.type)

	//test read/write
	SensorValueF32 sValNT(singleNT.type);
	VERIFY(sValNT.parseBinary(singleData1Binary))
	VERIFY(storage->writeSensorValue(&sValNT))
	VERIFY(storage->valuesCount()==1)
	SensorValue *sValGT2=storage->valueAt(0);
	VERIFY(sValGT2)
	VERIFY(sValGT2->type()==singleGT.type)
	VERIFY(((SensorValueF32*)sValGT2)->getSample(0)==singleData1)
	VERIFY(sValGT2->time()>=currentDT)
	delete sValGT2;

	//test open existing
	delete storage;
	iStorage=FSSensorStorageHelper::preOpen(storPath);
	VERIFY(iStorage)
	VERIFY(iStorage->open())
	VERIFY(iStorage->storeMode()==ISensorStorage::CONTINUOUS)
	VERIFY(iStorage->sensor()==singleNT)
	VERIFY(iStorage->storedValuesType()==singleGT.type)
	storage=(ContinuousStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==1)
	sValGT2=storage->valueAt(0);
	VERIFY(sValGT2)
	VERIFY(sValGT2->type()==singleGT.type)
	VERIFY(((SensorValueF32*)sValGT2)->getSample(0)==singleData1)
	VERIFY(sValGT2->time()>=currentDT)
	delete sValGT2;
}

void ContinuousStorageTests::testStorageSingleLTDontTouchTime()
{
	//continuous storage replaces LT by GT in any case
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ISensorStorage *iStorage=FSSensorStorageHelper::preCreate(
		storPath,deviceId,deviceName,singleLT,ISensorStorage::CONTINUOUS,ISensorStorage::DONT_TOUCH);
	VERIFY(iStorage->storeMode()==ISensorStorage::CONTINUOUS)
	VERIFY(iStorage->sensor()==singleLT)
	ContinuousStorage *storage=(ContinuousStorage*)iStorage;
	VERIFY(storage->create())
	VERIFY(storage->storedValuesType()==singleLT.type)

	//test read/write
	SensorValueF32 sValLT(singleLT.type);
	VERIFY(sValLT.parseMsgArgs(singleData1MsgArgsWithTs))
	VERIFY(storage->writeSensorValue(&sValLT))
	VERIFY(storage->valuesCount()==1)
	SensorValue *sValLT2=storage->valueAt(0);
	VERIFY(sValLT2)
	VERIFY(sValLT2->type()==singleLT.type)
	VERIFY(((SensorValueF32*)sValLT2)->getSample(0)==singleData1)
	VERIFY(sValLT2->time()==someTimestamp)
	delete sValLT2;

	//test open existing
	delete storage;
	iStorage=FSSensorStorageHelper::preOpen(storPath);
	VERIFY(iStorage)
	VERIFY(iStorage->open())
	VERIFY(iStorage->storeMode()==ISensorStorage::CONTINUOUS)
	VERIFY(iStorage->sensor()==singleLT)
	VERIFY(iStorage->storedValuesType()==singleLT.type)
	storage=(ContinuousStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==1)
	sValLT2=storage->valueAt(0);
	VERIFY(sValLT2)
	VERIFY(sValLT2->type()==singleLT.type)
	VERIFY(((SensorValueF32*)sValLT2)->getSample(0)==singleData1)
	VERIFY(sValLT2->time()==someTimestamp)
	delete sValLT2;
}

void ContinuousStorageTests::testStorageSingleGTDropTime()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ISensorStorage *iStorage=FSSensorStorageHelper::preCreate(
		storPath,deviceId,deviceName,singleGT,ISensorStorage::CONTINUOUS,ISensorStorage::DROP_TIME);
	VERIFY(iStorage->storeMode()==ISensorStorage::CONTINUOUS)
	VERIFY(iStorage->sensor()==singleGT)
	ContinuousStorage *storage=(ContinuousStorage*)iStorage;
	VERIFY(storage->create())
	VERIFY(storage->storedValuesType()==singleNT.type)

	//test read/write
	SensorValueF32 sValGT(singleGT.type);
	VERIFY(sValGT.parseMsgArgs(singleData1MsgArgsWithTs))
	VERIFY(storage->writeSensorValue(&sValGT))
	VERIFY(storage->valuesCount()==1)
	SensorValue *sValNT2=storage->valueAt(0);
	VERIFY(sValNT2)
	VERIFY(sValNT2->type()==singleNT.type)
	VERIFY(((SensorValueF32*)sValNT2)->getSample(0)==singleData1)
	delete sValNT2;

	//test open existing
	delete storage;
	iStorage=FSSensorStorageHelper::preOpen(storPath);
	VERIFY(iStorage)
	VERIFY(iStorage->open())
	VERIFY(iStorage->storeMode()==ISensorStorage::CONTINUOUS)
	VERIFY(iStorage->sensor()==singleGT)
	VERIFY(iStorage->storedValuesType()==singleNT.type)
	storage=(ContinuousStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==1)
	sValNT2=storage->valueAt(0);
	VERIFY(sValNT2)
	VERIFY(sValNT2->type()==singleNT.type)
	VERIFY(((SensorValueF32*)sValNT2)->getSample(0)==singleData1)
	delete sValNT2;
}

void ContinuousStorageTests::testStoragePacketNTDontTouchTime()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ISensorStorage *iStorage=FSSensorStorageHelper::preCreate(
		storPath,deviceId,deviceName,packetNT,ISensorStorage::CONTINUOUS,ISensorStorage::DONT_TOUCH);
	VERIFY(iStorage->storeMode()==ISensorStorage::CONTINUOUS)
	VERIFY(iStorage->sensor()==packetNT)
	ContinuousStorage *storage=(ContinuousStorage*)iStorage;
	VERIFY(storage->create())
	VERIFY(storage->storedValuesType()==packetNT.type)

	//test read/write
	SensorValueF32 pValNT(packetNT.type);
	VERIFY(pValNT.parseBinary(packetData1BinaryF))
	VERIFY(storage->writeSensorValue(&pValNT))
	VERIFY(storage->valuesCount()==1)
	SensorValue *pValNT2=storage->valueAt(0);
	VERIFY(pValNT2)
	VERIFY(pValNT2->type()==packetNT.type)
	SensorValueF32 *pValNT2F32=(SensorValueF32*)pValNT2;
	VERIFY(pValNT2F32->packetsCount()==2)
	VERIFY(pValNT2F32->getSample(0)==packetData1Sample0F)
	VERIFY(pValNT2F32->getSample(1)==packetData1Sample1F)
	delete pValNT2;

	//test open existing
	delete storage;
	iStorage=FSSensorStorageHelper::preOpen(storPath);
	VERIFY(iStorage)
	VERIFY(iStorage->open())
	VERIFY(iStorage->storeMode()==ISensorStorage::CONTINUOUS)
	VERIFY(iStorage->sensor()==packetNT)
	VERIFY(iStorage->storedValuesType()==packetNT.type)
	storage=(ContinuousStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==1)
	pValNT2=storage->valueAt(0);
	VERIFY(pValNT2)
	VERIFY(pValNT2->type()==packetNT.type)
	pValNT2F32=(SensorValueF32*)pValNT2;
	VERIFY(pValNT2F32->packetsCount()==2)
	VERIFY(pValNT2F32->getSample(0)==packetData1Sample0F)
	VERIFY(pValNT2F32->getSample(1)==packetData1Sample1F)
	delete pValNT2;
}

void ContinuousStorageTests::testStoragePacketGTDontTouchTime()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ISensorStorage *iStorage=FSSensorStorageHelper::preCreate(
		storPath,deviceId,deviceName,packetGT,ISensorStorage::CONTINUOUS,ISensorStorage::DONT_TOUCH);
	VERIFY(iStorage->storeMode()==ISensorStorage::CONTINUOUS)
	VERIFY(iStorage->sensor()==packetGT)
	ContinuousStorage *storage=(ContinuousStorage*)iStorage;
	VERIFY(storage->create())
	VERIFY(storage->storedValuesType()==packetGT.type)

	//test read/write
	SensorValueF32 pValGT(packetGT.type);
	pValGT.parseMsgArgs(packetData1MsgArgsWithTs);
	VERIFY(storage->writeSensorValue(&pValGT))
	VERIFY(storage->valuesCount()==1)
	SensorValue *pValGT2=storage->valueAt(0);
	VERIFY(pValGT2)
	VERIFY(pValGT2->type()==packetGT.type)
	VERIFY(pValGT2->packetsCount()==2)
	VERIFY(((SensorValueF32*)pValGT2)->getSample(0)==packetData1Sample0F)
	VERIFY(((SensorValueF32*)pValGT2)->getSample(1)==packetData1Sample1F)
	VERIFY(pValGT2->time()==someTimestamp)
	delete pValGT2;

	//test open existing
	delete storage;
	iStorage=FSSensorStorageHelper::preOpen(storPath);
	VERIFY(iStorage)
	VERIFY(iStorage->open())
	VERIFY(iStorage->storeMode()==ISensorStorage::CONTINUOUS)
	VERIFY(iStorage->sensor()==packetGT)
	VERIFY(iStorage->storedValuesType()==packetGT.type)
	storage=(ContinuousStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==1)
	pValGT2=storage->valueAt(0);
	VERIFY(pValGT2)
	VERIFY(pValGT2->type()==packetGT.type)
	VERIFY(((SensorValueF32*)pValGT2)->getSample(0)==packetData1Sample0F)
	VERIFY(((SensorValueF32*)pValGT2)->getSample(1)==packetData1Sample1F)
	VERIFY(pValGT2->time()==someTimestamp)
	delete pValGT2;
}

bool ContinuousStorageTests::init()
{
	QDir dir(storPath);
	dir.mkpath(dir.path());
	return true;
}

void ContinuousStorageTests::cleanup()
{
	RemoveDirRecusive::rmDirRec(QDir(storPath));
}
