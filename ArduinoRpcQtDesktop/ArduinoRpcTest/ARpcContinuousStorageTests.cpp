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
#include "ARpcBase/ARpcSingleSensorValue.h"
#include "ARpcBase/ARpcPacketSensorValue.h"
#include "ARpcBase/ARpcTextSensorValue.h"
#include <QDateTime>

static const QString storPath=QString("/tmp/ARpcTestStorage");
static const QVector<double> singleData1=QVector<double>()<<12.0<<621.124<<2845.125626;
static const QVector<double> singleData2=QVector<double>()<<156.6<<124.1<<0.1;
static const QVector<float> packetData1=QVector<float>()<<12.0<<621.124<<2845.125626<<156.6<<124.1<<0.1;

ARpcContinuousStorageTests::ARpcContinuousStorageTests(QObject *parent)
	:QtUnitTestSet("ARpcContinuousStorageTests",parent)
{
	addTest((TestFunction)&ARpcContinuousStorageTests::testStorageSingleDontTouchTime,
		"Test fixed blocks storage for SINGLE values with time policy DONT_TOUCH");
	addTest((TestFunction)&ARpcContinuousStorageTests::testStorageSingleAddGT,
		"Test chained blocks storage for SINGLE values with time policy ADD_GT");
	addTest((TestFunction)&ARpcContinuousStorageTests::testStorageSingleLTDontTouchTime,
		"Test fixed blocks storage for SINGLE_LT values with time policy DONT_TOUCH");
	addTest((TestFunction)&ARpcContinuousStorageTests::testStorageSingleLTAddGT,
		"Test fixed blocks storage for SINGLE_LT values with time policy ADD_GT");
	addTest((TestFunction)&ARpcContinuousStorageTests::testStorageSingleLTDropTime,
		"Test chained blocks storage for SINGLE_LT values with time policy DROP_TIME");
	addTest((TestFunction)&ARpcContinuousStorageTests::testStorageSingleGTDontTouchTime,
		"Test fixed blocks storage for SINGLE_GT values with time policy DONT_TOUCH");
	addTest((TestFunction)&ARpcContinuousStorageTests::testStorageSingleGTDropTime,
		"Test fixed blocks storage for SINGLE_GT values with time policy DROP_TIME");
	addTest((TestFunction)&ARpcContinuousStorageTests::testStoragePacketDontTouchTimeFixedBlocks,
		"Test fixed blocks storage for PACKET values with time policy DONT_TOUCH");
	addTest((TestFunction)&ARpcContinuousStorageTests::testStoragePacketGTDontTouchTimeFixedBlocks,
		"Test fixed blocks storage for PACKET_GT values with time policy DONT_TOUCH");
	addTest((TestFunction)&ARpcContinuousStorageTests::testStoragePacketGTDontTouchTimeChainedBlocks,
		"Test chained blocks storage for PACKET_GT values with time policy DONT_TOUCH");
}

void ARpcContinuousStorageTests::testStorageSingleDontTouchTime()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcISensorStorage::preCreate(
		storPath,ARpcISensorStorage::CONTINUOUS,ARpcSensor::SINGLE,ARpcISensorStorage::DONT_TOUCH);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE);
	ARpcContinuousStorage *storage=(ARpcContinuousStorage*)iStorage;
	VERIFY(storage->createAsFixedBlocksDb(ARpcSingleSensorValue(3)));
	VERIFY(storage->effectiveValuesType()==ARpcSensor::SINGLE);

	//test read/write
	ARpcSingleSensorValue sValNT(3);
	sValNT.fromData(singleData1);
	VERIFY(storage->writeSensorValue(&sValNT));
	VERIFY(storage->valuesCount()==1);
	ARpcSingleSensorValue *sValNT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==ARpcSensor::SINGLE)
	VERIFY(sValNT2->dims()==3);
	VERIFY(sValNT2->values()==singleData1);
	delete sValNT2;

	//test open existing
	delete storage;
	iStorage=ARpcISensorStorage::preOpen(storPath);
	VERIFY(iStorage);
	VERIFY(iStorage->open())
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE);
	VERIFY(iStorage->effectiveValuesType()==ARpcSensor::SINGLE);
	storage=(ARpcContinuousStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==1);
	sValNT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==ARpcSensor::SINGLE)
	VERIFY(sValNT2->dims()==3);
	VERIFY(sValNT2->values()==singleData1);
	delete sValNT2;
}

void ARpcContinuousStorageTests::testStorageSingleAddGT()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));
	qint64 currentDT=QDateTime::currentMSecsSinceEpoch();

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcISensorStorage::preCreate(
		storPath,ARpcISensorStorage::CONTINUOUS,ARpcSensor::SINGLE,ARpcISensorStorage::ADD_GT);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE);
	ARpcContinuousStorage *storage=(ARpcContinuousStorage*)iStorage;
	VERIFY(storage->createAsChainedBlocksDb());
	VERIFY(storage->effectiveValuesType()==ARpcSensor::SINGLE_GT);

	//test read/write
	ARpcSingleSensorValue sValNT(3);
	sValNT.fromData(singleData1);
	VERIFY(storage->writeSensorValue(&sValNT));
	VERIFY(storage->valuesCount()==1);
	ARpcSingleSensorValue *sValGT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValGT2);
	VERIFY(sValGT2->type()==ARpcSensor::SINGLE_GT)
	VERIFY(sValGT2->dims()==3);
	VERIFY(sValGT2->values()==singleData1);
	VERIFY(sValGT2->time()>=currentDT);
	delete sValGT2;

	//test open existing
	delete storage;
	iStorage=ARpcISensorStorage::preOpen(storPath);
	VERIFY(iStorage);
	VERIFY(iStorage->open())
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE);
	VERIFY(iStorage->effectiveValuesType()==ARpcSensor::SINGLE_GT);
	storage=(ARpcContinuousStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==1);
	sValGT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValGT2);
	VERIFY(sValGT2->type()==ARpcSensor::SINGLE_GT)
	VERIFY(sValGT2->dims()==3);
	VERIFY(sValGT2->values()==singleData1);
	VERIFY(sValGT2->time()>=currentDT);
	delete sValGT2;
}

void ARpcContinuousStorageTests::testStorageSingleLTDontTouchTime()
{
	//continuous storage replaces LT by GT in any case
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));
	qint64 currentDT=QDateTime::currentMSecsSinceEpoch();

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcISensorStorage::preCreate(
		storPath,ARpcISensorStorage::CONTINUOUS,ARpcSensor::SINGLE_LT,ARpcISensorStorage::DONT_TOUCH);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE_LT);
	ARpcContinuousStorage *storage=(ARpcContinuousStorage*)iStorage;
	VERIFY(storage->createAsFixedBlocksDb(ARpcSingleSensorValue(3,true)));
	VERIFY(storage->effectiveValuesType()==ARpcSensor::SINGLE_GT);

	//test read/write
	ARpcSingleSensorValue sValLT(3,true);
	sValLT.fromData(singleData1);
	sValLT.setTime(10);
	VERIFY(storage->writeSensorValue(&sValLT));
	VERIFY(storage->valuesCount()==1);
	ARpcSingleSensorValue *sValGT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValGT2);
	VERIFY(sValGT2->type()==ARpcSensor::SINGLE_GT);
	VERIFY(sValGT2->dims()==3);
	VERIFY(sValGT2->values()==singleData1);
	VERIFY(sValGT2->time()>=currentDT)
	delete sValGT2;

	//test open existing
	delete storage;
	iStorage=ARpcISensorStorage::preOpen(storPath);
	VERIFY(iStorage);
	VERIFY(iStorage->open())
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE_LT);
	VERIFY(iStorage->effectiveValuesType()==ARpcSensor::SINGLE_GT);
	storage=(ARpcContinuousStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==1);
	sValGT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValGT2);
	VERIFY(sValGT2->type()==ARpcSensor::SINGLE_GT);
	VERIFY(sValGT2->dims()==3);
	VERIFY(sValGT2->values()==singleData1);
	VERIFY(sValGT2->time()>=currentDT);
	delete sValGT2;
}

void ARpcContinuousStorageTests::testStorageSingleLTAddGT()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));
	qint64 currentDT=QDateTime::currentMSecsSinceEpoch();

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcISensorStorage::preCreate(
		storPath,ARpcISensorStorage::CONTINUOUS,ARpcSensor::SINGLE_LT,ARpcISensorStorage::ADD_GT);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE_LT);
	ARpcContinuousStorage *storage=(ARpcContinuousStorage*)iStorage;
	VERIFY(storage->createAsFixedBlocksDb(ARpcSingleSensorValue(3,true)));
	VERIFY(storage->effectiveValuesType()==ARpcSensor::SINGLE_GT);

	//test read/write
	ARpcSingleSensorValue sValLT(3,true);
	sValLT.fromData(singleData1);
	sValLT.setTime(10);
	VERIFY(storage->writeSensorValue(&sValLT));
	VERIFY(storage->valuesCount()==1);
	ARpcSingleSensorValue *sValGT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValGT2);
	VERIFY(sValGT2->type()==ARpcSensor::SINGLE_GT);
	VERIFY(sValGT2->dims()==3);
	VERIFY(sValGT2->values()==singleData1);
	VERIFY(sValGT2->time()>=currentDT)
	delete sValGT2;

	//test open existing
	delete storage;
	iStorage=ARpcISensorStorage::preOpen(storPath);
	VERIFY(iStorage);
	VERIFY(iStorage->open())
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE_LT);
	VERIFY(iStorage->effectiveValuesType()==ARpcSensor::SINGLE_GT);
	storage=(ARpcContinuousStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==1);
	sValGT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValGT2);
	VERIFY(sValGT2->type()==ARpcSensor::SINGLE_GT);
	VERIFY(sValGT2->dims()==3);
	VERIFY(sValGT2->values()==singleData1);
	VERIFY(sValGT2->time()>=currentDT);
	delete sValGT2;
}

void ARpcContinuousStorageTests::testStorageSingleLTDropTime()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcISensorStorage::preCreate(
		storPath,ARpcISensorStorage::CONTINUOUS,ARpcSensor::SINGLE_LT,ARpcISensorStorage::DROP_TIME);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE_LT);
	ARpcContinuousStorage *storage=(ARpcContinuousStorage*)iStorage;
	VERIFY(storage->createAsChainedBlocksDb());
	VERIFY(storage->effectiveValuesType()==ARpcSensor::SINGLE);

	//test read/write
	ARpcSingleSensorValue sValLT(3,true);
	sValLT.fromData(singleData1);
	sValLT.setTime(10);
	VERIFY(storage->writeSensorValue(&sValLT));
	VERIFY(storage->valuesCount()==1);
	ARpcSingleSensorValue *sValNT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==ARpcSensor::SINGLE);
	VERIFY(sValNT2->dims()==3);
	VERIFY(sValNT2->values()==singleData1);
	delete sValNT2;

	//test open existing
	delete storage;
	iStorage=ARpcISensorStorage::preOpen(storPath);
	VERIFY(iStorage);
	VERIFY(iStorage->open())
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE_LT);
	VERIFY(iStorage->effectiveValuesType()==ARpcSensor::SINGLE);
	storage=(ARpcContinuousStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==1);
	sValNT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==ARpcSensor::SINGLE);
	VERIFY(sValNT2->dims()==3);
	VERIFY(sValNT2->values()==singleData1);
	delete sValNT2;
}

void ARpcContinuousStorageTests::testStorageSingleGTDontTouchTime()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcISensorStorage::preCreate(
		storPath,ARpcISensorStorage::CONTINUOUS,ARpcSensor::SINGLE_GT,ARpcISensorStorage::DONT_TOUCH);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE_GT);
	ARpcContinuousStorage *storage=(ARpcContinuousStorage*)iStorage;
	VERIFY(storage->createAsFixedBlocksDb(ARpcSingleSensorValue(3,false)));
	VERIFY(storage->effectiveValuesType()==ARpcSensor::SINGLE_GT);

	//test read/write
	ARpcSingleSensorValue sValGT(3,false);
	sValGT.fromData(singleData1);
	sValGT.setTime(10);
	VERIFY(storage->writeSensorValue(&sValGT));
	VERIFY(storage->valuesCount()==1);
	ARpcSingleSensorValue *sValGT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValGT2);
	VERIFY(sValGT2->type()==ARpcSensor::SINGLE_GT);
	VERIFY(sValGT2->dims()==3);
	VERIFY(sValGT2->values()==singleData1);
	VERIFY(sValGT2->time()==sValGT.time())
	delete sValGT2;

	//test open existing
	delete storage;
	iStorage=ARpcISensorStorage::preOpen(storPath);
	VERIFY(iStorage);
	VERIFY(iStorage->open())
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE_GT);
	VERIFY(iStorage->effectiveValuesType()==ARpcSensor::SINGLE_GT);
	storage=(ARpcContinuousStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==1);
	sValGT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValGT2);
	VERIFY(sValGT2->type()==ARpcSensor::SINGLE_GT);
	VERIFY(sValGT2->dims()==3);
	VERIFY(sValGT2->values()==singleData1);
	VERIFY(sValGT2->time()==sValGT.time());
	delete sValGT2;
}

void ARpcContinuousStorageTests::testStorageSingleGTDropTime()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcISensorStorage::preCreate(
		storPath,ARpcISensorStorage::CONTINUOUS,ARpcSensor::SINGLE_GT,ARpcISensorStorage::DROP_TIME);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE_GT);
	ARpcContinuousStorage *storage=(ARpcContinuousStorage*)iStorage;
	VERIFY(storage->createAsFixedBlocksDb(ARpcSingleSensorValue(3,false)));
	VERIFY(storage->effectiveValuesType()==ARpcSensor::SINGLE);

	//test read/write
	ARpcSingleSensorValue sValGT(3,false);
	sValGT.fromData(singleData1);
	sValGT.setTime(10);
	VERIFY(storage->writeSensorValue(&sValGT));
	VERIFY(storage->valuesCount()==1);
	ARpcSingleSensorValue *sValNT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==ARpcSensor::SINGLE);
	VERIFY(sValNT2->dims()==3);
	VERIFY(sValNT2->values()==singleData1);
	delete sValNT2;

	//test open existing
	delete storage;
	iStorage=ARpcISensorStorage::preOpen(storPath);
	VERIFY(iStorage);
	VERIFY(iStorage->open())
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE_GT);
	VERIFY(iStorage->effectiveValuesType()==ARpcSensor::SINGLE);
	storage=(ARpcContinuousStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==1);
	sValNT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==ARpcSensor::SINGLE);
	VERIFY(sValNT2->dims()==3);
	VERIFY(sValNT2->values()==singleData1);
	delete sValNT2;
}

void ARpcContinuousStorageTests::testStoragePacketDontTouchTimeFixedBlocks()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcISensorStorage::preCreate(
		storPath,ARpcISensorStorage::CONTINUOUS,ARpcSensor::PACKET,ARpcISensorStorage::DONT_TOUCH);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::PACKET);
	ARpcContinuousStorage *storage=(ARpcContinuousStorage*)iStorage;
	ARpcPacketSensorValue vvv(3);
	vvv.fromData(packetData1,3);
	VERIFY(storage->createAsFixedBlocksDb(vvv));
	VERIFY(storage->effectiveValuesType()==ARpcSensor::PACKET);

	//test read/write
	ARpcPacketSensorValue pValNT(3);
	pValNT.fromData(packetData1,3);
	VERIFY(storage->writeSensorValue(&pValNT));
	VERIFY(storage->valuesCount()==1);
	ARpcPacketSensorValue *pValNT2=(ARpcPacketSensorValue*)storage->valueAt(0);
	VERIFY(pValNT2);
	VERIFY(pValNT2->type()==ARpcSensor::PACKET)
	VERIFY(pValNT2->dims()==3);
	VERIFY(pValNT2->values()==packetData1);
	delete pValNT2;

	//test open existing
	delete storage;
	iStorage=ARpcISensorStorage::preOpen(storPath);
	VERIFY(iStorage);
	VERIFY(iStorage->open())
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::PACKET);
	VERIFY(iStorage->effectiveValuesType()==ARpcSensor::PACKET);
	storage=(ARpcContinuousStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==1);
	pValNT2=(ARpcPacketSensorValue*)storage->valueAt(0);
	VERIFY(pValNT2);
	VERIFY(pValNT2->type()==ARpcSensor::PACKET)
	VERIFY(pValNT2->dims()==3);
	VERIFY(pValNT2->values()==packetData1);
	delete pValNT2;
}

void ARpcContinuousStorageTests::testStoragePacketGTDontTouchTimeFixedBlocks()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcISensorStorage::preCreate(
		storPath,ARpcISensorStorage::CONTINUOUS,ARpcSensor::PACKET_GT,ARpcISensorStorage::DONT_TOUCH);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::PACKET_GT);
	ARpcContinuousStorage *storage=(ARpcContinuousStorage*)iStorage;
	ARpcPacketSensorValue vvv(3,false);
	vvv.fromData(packetData1,3);
	VERIFY(storage->createAsFixedBlocksDb(vvv));
	VERIFY(storage->effectiveValuesType()==ARpcSensor::PACKET_GT);

	//test read/write
	ARpcPacketSensorValue pValGT(3,false);
	pValGT.fromData(packetData1,3);
	pValGT.setTime(10);
	VERIFY(storage->writeSensorValue(&pValGT));
	VERIFY(storage->valuesCount()==1);
	ARpcPacketSensorValue *pValGT2=(ARpcPacketSensorValue*)storage->valueAt(0);
	VERIFY(pValGT2);
	VERIFY(pValGT2->type()==ARpcSensor::PACKET_GT)
	VERIFY(pValGT2->dims()==3);
	VERIFY(pValGT2->values()==packetData1);
	VERIFY(pValGT2->time()==pValGT.time());
	delete pValGT2;

	//test open existing
	delete storage;
	iStorage=ARpcISensorStorage::preOpen(storPath);
	VERIFY(iStorage);
	VERIFY(iStorage->open())
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::PACKET_GT);
	VERIFY(iStorage->effectiveValuesType()==ARpcSensor::PACKET_GT);
	storage=(ARpcContinuousStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==1);
	pValGT2=(ARpcPacketSensorValue*)storage->valueAt(0);
	VERIFY(pValGT2);
	VERIFY(pValGT2->type()==ARpcSensor::PACKET_GT)
	VERIFY(pValGT2->dims()==3);
	VERIFY(pValGT2->values()==packetData1);
	VERIFY(pValGT2->time()==pValGT.time());
	delete pValGT2;
}

void ARpcContinuousStorageTests::testStoragePacketGTDontTouchTimeChainedBlocks()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcISensorStorage::preCreate(
		storPath,ARpcISensorStorage::CONTINUOUS,ARpcSensor::PACKET_GT,ARpcISensorStorage::DONT_TOUCH);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::PACKET_GT);
	ARpcContinuousStorage *storage=(ARpcContinuousStorage*)iStorage;
	ARpcPacketSensorValue vvv(3,false);
	vvv.fromData(packetData1,3);
	VERIFY(storage->createAsFixedBlocksDb(vvv));
	VERIFY(storage->effectiveValuesType()==ARpcSensor::PACKET_GT);

	//test read/write
	ARpcPacketSensorValue pValGT(3,false);
	pValGT.fromData(packetData1,3);
	pValGT.setTime(10);
	VERIFY(storage->writeSensorValue(&pValGT));
	VERIFY(storage->valuesCount()==1);
	ARpcPacketSensorValue *pValGT2=(ARpcPacketSensorValue*)storage->valueAt(0);
	VERIFY(pValGT2);
	VERIFY(pValGT2->type()==ARpcSensor::PACKET_GT)
	VERIFY(pValGT2->dims()==3);
	VERIFY(pValGT2->values()==packetData1);
	VERIFY(pValGT2->time()==pValGT.time());
	delete pValGT2;

	//test open existing
	delete storage;
	iStorage=ARpcISensorStorage::preOpen(storPath);
	VERIFY(iStorage);
	VERIFY(iStorage->open())
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::PACKET_GT);
	VERIFY(iStorage->effectiveValuesType()==ARpcSensor::PACKET_GT);
	storage=(ARpcContinuousStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==1);
	pValGT2=(ARpcPacketSensorValue*)storage->valueAt(0);
	VERIFY(pValGT2);
	VERIFY(pValGT2->type()==ARpcSensor::PACKET_GT)
	VERIFY(pValGT2->dims()==3);
	VERIFY(pValGT2->values()==packetData1);
	VERIFY(pValGT2->time()==pValGT.time());
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
