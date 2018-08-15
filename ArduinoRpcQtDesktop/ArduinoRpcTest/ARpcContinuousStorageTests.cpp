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

#include "ARpcContinuousStorageTests.h"
#include "RemoveDirRecusive.h"
#include "ARpcLocalStorage/ARpcContinuousStorage.h"
#include "TestData.h"
#include <QDateTime>

ARpcContinuousStorageTests::ARpcContinuousStorageTests(QObject *parent)
	:QtUnitTestSet("ARpcContinuousStorageTests",parent)
{
	addTest((TestFunction)&ARpcContinuousStorageTests::testStorageSingleDontTouchTime,
		"Test storage for single values with time policy DONT_TOUCH");
	addTest((TestFunction)&ARpcContinuousStorageTests::testStorageSingleAddGT,
		"Test storage for single values with time policy ADD_GT");
	addTest((TestFunction)&ARpcContinuousStorageTests::testStorageSingleLTDontTouchTime,
		"Test storage for single values with local timestamps with time policy DONT_TOUCH");
	addTest((TestFunction)&ARpcContinuousStorageTests::testStorageSingleGTDropTime,
		"Test storage for single values with global timestamps with time policy DROP_TIME");
	addTest((TestFunction)&ARpcContinuousStorageTests::testStoragePacketNTDontTouchTime,
		"Test storage for packet values with time policy DONT_TOUCH");
	addTest((TestFunction)&ARpcContinuousStorageTests::testStoragePacketGTDontTouchTime,
		"Test storage for packet values with global timestamp with time policy DONT_TOUCH");
}

void ARpcContinuousStorageTests::testStorageSingleDontTouchTime()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcISensorStorage::preCreate(
		storPath,ARpcISensorStorage::CONTINUOUS,singleNT,deviceId,deviceName,ARpcISensorStorage::DONT_TOUCH);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensor()==singleNT);
	ARpcContinuousStorage *storage=(ARpcContinuousStorage*)iStorage;
	VERIFY(storage->create());
	VERIFY(storage->effectiveValuesType()==singleNT.type);

	//test read/write
	ARpcSensorValueF32 sValNT(singleNT.type);
	sValNT.parseBinary(singleData1Binary);
	VERIFY(storage->writeSensorValue(&sValNT));
	VERIFY(storage->valuesCount()==1);
	ARpcSensorValue *sValNT2=storage->valueAt(0);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==singleNT.type)
	VERIFY(((ARpcSensorValueF32*)sValNT2)->getSample()==singleData1);
	delete sValNT2;

	//test open existing
	delete storage;
	iStorage=ARpcISensorStorage::preOpen(storPath);
	VERIFY(iStorage);
	VERIFY(iStorage->open())
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensor()==singleNT);
	VERIFY(iStorage->effectiveValuesType()==singleNT.type);
	storage=(ARpcContinuousStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==1);
	sValNT2=storage->valueAt(0);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==singleNT.type)
	VERIFY(((ARpcSensorValueF32*)sValNT2)->getSample()==singleData1);
	delete sValNT2;
}

void ARpcContinuousStorageTests::testStorageSingleAddGT()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));
	qint64 currentDT=QDateTime::currentMSecsSinceEpoch();

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcISensorStorage::preCreate(
		storPath,ARpcISensorStorage::CONTINUOUS,singleNT,deviceId,deviceName,ARpcISensorStorage::ADD_GT);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensor()==singleNT);
	ARpcContinuousStorage *storage=(ARpcContinuousStorage*)iStorage;
	VERIFY(storage->create());
	VERIFY(storage->effectiveValuesType()==singleGT.type);

	//test read/write
	ARpcSensorValueF32 sValNT(singleNT.type);
	VERIFY(sValNT.parseBinary(singleData1Binary));
	VERIFY(storage->writeSensorValue(&sValNT));
	VERIFY(storage->valuesCount()==1);
	ARpcSensorValue *sValGT2=storage->valueAt(0);
	VERIFY(sValGT2);
	VERIFY(sValGT2->type()==singleGT.type)
	VERIFY(((ARpcSensorValueF32*)sValGT2)->getSample()==singleData1);
	VERIFY(sValGT2->time()>=currentDT);
	delete sValGT2;

	//test open existing
	delete storage;
	iStorage=ARpcISensorStorage::preOpen(storPath);
	VERIFY(iStorage);
	VERIFY(iStorage->open())
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensor()==singleNT);
	VERIFY(iStorage->effectiveValuesType()==singleGT.type);
	storage=(ARpcContinuousStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==1);
	sValGT2=storage->valueAt(0);
	VERIFY(sValGT2);
	VERIFY(sValGT2->type()==singleGT.type)
	VERIFY(((ARpcSensorValueF32*)sValGT2)->getSample()==singleData1);
	VERIFY(sValGT2->time()>=currentDT);
	delete sValGT2;
}

void ARpcContinuousStorageTests::testStorageSingleLTDontTouchTime()
{
	//continuous storage replaces LT by GT in any case
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcISensorStorage::preCreate(
		storPath,ARpcISensorStorage::CONTINUOUS,singleLT,deviceId,deviceName,ARpcISensorStorage::DONT_TOUCH);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensor()==singleLT);
	ARpcContinuousStorage *storage=(ARpcContinuousStorage*)iStorage;
	VERIFY(storage->create());
	VERIFY(storage->effectiveValuesType()==singleLT.type);

	//test read/write
	ARpcSensorValueF32 sValLT(singleLT.type);
	VERIFY(sValLT.parseMsgArgs(singleData1MsgArgsWithTs));
	VERIFY(storage->writeSensorValue(&sValLT));
	VERIFY(storage->valuesCount()==1);
	ARpcSensorValue *sValLT2=storage->valueAt(0);
	VERIFY(sValLT2);
	VERIFY(sValLT2->type()==singleLT.type);
	VERIFY(((ARpcSensorValueF32*)sValLT2)->getSample()==singleData1);
	VERIFY(sValLT2->time()==someTimestamp);
	delete sValLT2;

	//test open existing
	delete storage;
	iStorage=ARpcISensorStorage::preOpen(storPath);
	VERIFY(iStorage);
	VERIFY(iStorage->open())
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensor()==singleLT);
	VERIFY(iStorage->effectiveValuesType()==singleLT.type);
	storage=(ARpcContinuousStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==1);
	sValLT2=storage->valueAt(0);
	VERIFY(sValLT2);
	VERIFY(sValLT2->type()==singleLT.type);
	VERIFY(((ARpcSensorValueF32*)sValLT2)->getSample()==singleData1);
	VERIFY(sValLT2->time()==someTimestamp);
	delete sValLT2;
}

void ARpcContinuousStorageTests::testStorageSingleGTDropTime()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcISensorStorage::preCreate(
		storPath,ARpcISensorStorage::CONTINUOUS,singleGT,deviceId,deviceName,ARpcISensorStorage::DROP_TIME);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensor()==singleGT);
	ARpcContinuousStorage *storage=(ARpcContinuousStorage*)iStorage;
	VERIFY(storage->create());
	VERIFY(storage->effectiveValuesType()==singleNT.type);

	//test read/write
	ARpcSensorValueF32 sValGT(singleGT.type);
	VERIFY(sValGT.parseMsgArgs(singleData1MsgArgsWithTs));
	VERIFY(storage->writeSensorValue(&sValGT));
	VERIFY(storage->valuesCount()==1);
	ARpcSensorValue *sValNT2=storage->valueAt(0);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==singleNT.type);
	VERIFY(((ARpcSensorValueF32*)sValNT2)->getSample()==singleData1);
	delete sValNT2;

	//test open existing
	delete storage;
	iStorage=ARpcISensorStorage::preOpen(storPath);
	VERIFY(iStorage);
	VERIFY(iStorage->open())
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensor()==singleGT);
	VERIFY(iStorage->effectiveValuesType()==singleNT.type);
	storage=(ARpcContinuousStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==1);
	sValNT2=storage->valueAt(0);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==singleNT.type);
	VERIFY(((ARpcSensorValueF32*)sValNT2)->getSample()==singleData1);
	delete sValNT2;
}

void ARpcContinuousStorageTests::testStoragePacketNTDontTouchTime()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcISensorStorage::preCreate(
		storPath,ARpcISensorStorage::CONTINUOUS,packetNT,deviceId,deviceName,ARpcISensorStorage::DONT_TOUCH);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensor()==packetNT);
	ARpcContinuousStorage *storage=(ARpcContinuousStorage*)iStorage;
	VERIFY(storage->create());
	VERIFY(storage->effectiveValuesType()==packetNT.type);

	//test read/write
	ARpcSensorValueF32 pValNT(packetNT.type);
	VERIFY(pValNT.parseBinary(packetData1BinaryF));
	VERIFY(storage->writeSensorValue(&pValNT));
	VERIFY(storage->valuesCount()==1);
	ARpcSensorValue *pValNT2=storage->valueAt(0);
	VERIFY(pValNT2);
	VERIFY(pValNT2->type()==packetNT.type)
	ARpcSensorValueF32 *pValNT2F32=(ARpcSensorValueF32*)pValNT2;
	VERIFY(pValNT2F32->packetsCount()==2)
	VERIFY(pValNT2F32->getSample(0)==packetData1Sample0F);
	VERIFY(pValNT2F32->getSample(1)==packetData1Sample1F);
	delete pValNT2;

	//test open existing
	delete storage;
	iStorage=ARpcISensorStorage::preOpen(storPath);
	VERIFY(iStorage);
	VERIFY(iStorage->open())
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensor()==packetNT);
	VERIFY(iStorage->effectiveValuesType()==packetNT.type);
	storage=(ARpcContinuousStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==1);
	pValNT2=storage->valueAt(0);
	VERIFY(pValNT2);
	VERIFY(pValNT2->type()==packetNT.type)
	pValNT2F32=(ARpcSensorValueF32*)pValNT2;
	VERIFY(pValNT2F32->packetsCount()==2)
	VERIFY(pValNT2F32->getSample(0)==packetData1Sample0F);
	VERIFY(pValNT2F32->getSample(1)==packetData1Sample1F);
	delete pValNT2;
}

void ARpcContinuousStorageTests::testStoragePacketGTDontTouchTime()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcISensorStorage::preCreate(
		storPath,ARpcISensorStorage::CONTINUOUS,packetGT,deviceId,deviceName,ARpcISensorStorage::DONT_TOUCH);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensor()==packetGT);
	ARpcContinuousStorage *storage=(ARpcContinuousStorage*)iStorage;
	VERIFY(storage->create());
	VERIFY(storage->effectiveValuesType()==packetGT.type);

	//test read/write
	ARpcSensorValueF32 pValGT(packetGT.type);
	pValGT.parseMsgArgs(packetData1MsgArgsWithTs);
	VERIFY(storage->writeSensorValue(&pValGT));
	VERIFY(storage->valuesCount()==1);
	ARpcSensorValue *pValGT2=storage->valueAt(0);
	VERIFY(pValGT2);
	VERIFY(pValGT2->type()==packetGT.type)
	VERIFY(pValGT2->packetsCount()==2)
	VERIFY(((ARpcSensorValueF32*)pValGT2)->getSample(0)==packetData1Sample0F);
	VERIFY(((ARpcSensorValueF32*)pValGT2)->getSample(1)==packetData1Sample1F);
	VERIFY(pValGT2->time()==someTimestamp);
	delete pValGT2;

	//test open existing
	delete storage;
	iStorage=ARpcISensorStorage::preOpen(storPath);
	VERIFY(iStorage);
	VERIFY(iStorage->open())
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensor()==packetGT);
	VERIFY(iStorage->effectiveValuesType()==packetGT.type);
	storage=(ARpcContinuousStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==1);
	pValGT2=storage->valueAt(0);
	VERIFY(pValGT2);
	VERIFY(pValGT2->type()==packetGT.type)
	VERIFY(((ARpcSensorValueF32*)pValGT2)->getSample(0)==packetData1Sample0F);
	VERIFY(((ARpcSensorValueF32*)pValGT2)->getSample(1)==packetData1Sample1F);
	VERIFY(pValGT2->time()==someTimestamp);
	delete pValGT2;
}

bool ARpcContinuousStorageTests::init()
{
	QDir dir(storPath);
	dir.mkpath(dir.path());
	return true;
}

void ARpcContinuousStorageTests::cleanup()
{
	RemoveDirRecusive::rmDirRec(QDir(storPath));
}
