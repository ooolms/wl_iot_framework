#include "ARpcContinuousStorageTests.h"
#include "RemoveDirRecusive.h"
#include "ARpcLocalStorage/ARpcContinuousStorage.h"
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

ARpcContinuousStorageTests::ARpcContinuousStorageTests(QObject *parent)
	:QtUnitTestSet("ARpcContinuousStorageTests",parent)
{
	addTest((TestFunction)&ARpcContinuousStorageTests::testStorageSingleDontTouchTime,
		"Test storage for SINGLE values with time policy DONT_TOUCH");
	addTest((TestFunction)&ARpcContinuousStorageTests::testStorageSingleAddGT,
		"Test storage for SINGLE values with time policy ADD_GT");
	addTest((TestFunction)&ARpcContinuousStorageTests::testStorageSingleLTDontTouchTime,
		"Test storage for SINGLE_LT values with time policy DONT_TOUCH");
	addTest((TestFunction)&ARpcContinuousStorageTests::testStorageSingleLTAddGT,
		"Test storage for SINGLE_LT values with time policy ADD_GT");
	addTest((TestFunction)&ARpcContinuousStorageTests::testStorageSingleLTDropTime,
		"Test storage for SINGLE_LT values with time policy DROP_TIME");
	addTest((TestFunction)&ARpcContinuousStorageTests::testStorageSingleGTDontTouchTime,
		"Test storage for SINGLE_GT values with time policy DONT_TOUCH");
	addTest((TestFunction)&ARpcContinuousStorageTests::testStorageSingleGTDropTime,
		"Test storage for SINGLE_GT values with time policy DROP_TIME");
}

void ARpcContinuousStorageTests::testStorageSingleDontTouchTime()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcISensorStorage::preCreate(
		storPath,ARpcISensorStorage::CONTINUOUS,ARpcSensor::SINGLE);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE);
	ARpcContinuousStorage *storage=(ARpcContinuousStorage*)iStorage;
	VERIFY(storage->createAsFixedBlocksDb(ARpcSingleSensorValue(3),ARpcISensorStorage::DONT_TOUCH));
	VERIFY(storage->effectiveValuesType()==ARpcSensor::SINGLE);

	//test read/write
	ARpcSingleSensorValue sValNT(3);
	sValNT.fromData(testData1);
	VERIFY(storage->writeSensorValue(&sValNT));
	VERIFY(storage->valuesCount()==1);
	ARpcSingleSensorValue *sValNT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==ARpcSensor::SINGLE)
	VERIFY(sValNT2->dims()==3);
	VERIFY(sValNT2->values()==testData1);
	delete sValNT2;

	//test open existing
	delete storage;
	iStorage=ARpcISensorStorage::open(storPath);
	VERIFY(iStorage);
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
	VERIFY(sValNT2->values()==testData1);
	delete sValNT2;
}

void ARpcContinuousStorageTests::testStorageSingleAddGT()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));
	qint64 currentDT=QDateTime::currentMSecsSinceEpoch();

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcISensorStorage::preCreate(
		storPath,ARpcISensorStorage::CONTINUOUS,ARpcSensor::SINGLE);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE);
	ARpcContinuousStorage *storage=(ARpcContinuousStorage*)iStorage;
	VERIFY(storage->createAsFixedBlocksDb(ARpcSingleSensorValue(3),ARpcISensorStorage::ADD_GT));
	VERIFY(storage->effectiveValuesType()==ARpcSensor::SINGLE_GT);

	//test read/write
	ARpcSingleSensorValue sValNT(3);
	sValNT.fromData(testData1);
	VERIFY(storage->writeSensorValue(&sValNT));
	VERIFY(storage->valuesCount()==1);
	ARpcSingleSensorValue *sValGT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValGT2);
	VERIFY(sValGT2->type()==ARpcSensor::SINGLE_GT)
	VERIFY(sValGT2->dims()==3);
	VERIFY(sValGT2->values()==testData1);
	VERIFY(sValGT2->time()>=currentDT);
	delete sValGT2;

	//test open existing
	delete storage;
	iStorage=ARpcISensorStorage::open(storPath);
	VERIFY(iStorage);
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
	VERIFY(sValGT2->values()==testData1);
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
		storPath,ARpcISensorStorage::CONTINUOUS,ARpcSensor::SINGLE_LT);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE_LT);
	ARpcContinuousStorage *storage=(ARpcContinuousStorage*)iStorage;
	VERIFY(storage->createAsFixedBlocksDb(ARpcSingleSensorValue(3,true),ARpcISensorStorage::DONT_TOUCH));
	VERIFY(storage->effectiveValuesType()==ARpcSensor::SINGLE_GT);

	//test read/write
	ARpcSingleSensorValue sValLT(3,true);
	sValLT.fromData(testData1);
	sValLT.setTime(10);
	VERIFY(storage->writeSensorValue(&sValLT));
	VERIFY(storage->valuesCount()==1);
	ARpcSingleSensorValue *sValGT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValGT2);
	VERIFY(sValGT2->type()==ARpcSensor::SINGLE_GT);
	VERIFY(sValGT2->dims()==3);
	VERIFY(sValGT2->values()==testData1);
	VERIFY(sValGT2->time()>=currentDT)
	delete sValGT2;

	//test open existing
	delete storage;
	iStorage=ARpcISensorStorage::open(storPath);
	VERIFY(iStorage);
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
	VERIFY(sValGT2->values()==testData1);
	VERIFY(sValGT2->time()>=currentDT);
	delete sValGT2;
}

void ARpcContinuousStorageTests::testStorageSingleLTAddGT()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));
	qint64 currentDT=QDateTime::currentMSecsSinceEpoch();

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcISensorStorage::preCreate(
		storPath,ARpcISensorStorage::CONTINUOUS,ARpcSensor::SINGLE_LT);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE_LT);
	ARpcContinuousStorage *storage=(ARpcContinuousStorage*)iStorage;
	VERIFY(storage->createAsFixedBlocksDb(ARpcSingleSensorValue(3,true),ARpcISensorStorage::ADD_GT));
	VERIFY(storage->effectiveValuesType()==ARpcSensor::SINGLE_GT);

	//test read/write
	ARpcSingleSensorValue sValLT(3,true);
	sValLT.fromData(testData1);
	sValLT.setTime(10);
	VERIFY(storage->writeSensorValue(&sValLT));
	VERIFY(storage->valuesCount()==1);
	ARpcSingleSensorValue *sValGT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValGT2);
	VERIFY(sValGT2->type()==ARpcSensor::SINGLE_GT);
	VERIFY(sValGT2->dims()==3);
	VERIFY(sValGT2->values()==testData1);
	VERIFY(sValGT2->time()>=currentDT)
	delete sValGT2;

	//test open existing
	delete storage;
	iStorage=ARpcISensorStorage::open(storPath);
	VERIFY(iStorage);
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
	VERIFY(sValGT2->values()==testData1);
	VERIFY(sValGT2->time()>=currentDT);
	delete sValGT2;
}

void ARpcContinuousStorageTests::testStorageSingleLTDropTime()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcISensorStorage::preCreate(
		storPath,ARpcISensorStorage::CONTINUOUS,ARpcSensor::SINGLE_LT);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE_LT);
	ARpcContinuousStorage *storage=(ARpcContinuousStorage*)iStorage;
	VERIFY(storage->createAsFixedBlocksDb(ARpcSingleSensorValue(3,true),ARpcISensorStorage::DROP_TIME));
	VERIFY(storage->effectiveValuesType()==ARpcSensor::SINGLE);

	//test read/write
	ARpcSingleSensorValue sValLT(3,true);
	sValLT.fromData(testData1);
	sValLT.setTime(10);
	VERIFY(storage->writeSensorValue(&sValLT));
	VERIFY(storage->valuesCount()==1);
	ARpcSingleSensorValue *sValNT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==ARpcSensor::SINGLE);
	VERIFY(sValNT2->dims()==3);
	VERIFY(sValNT2->values()==testData1);
	delete sValNT2;

	//test open existing
	delete storage;
	iStorage=ARpcISensorStorage::open(storPath);
	VERIFY(iStorage);
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
	VERIFY(sValNT2->values()==testData1);
	delete sValNT2;
}

void ARpcContinuousStorageTests::testStorageSingleGTDontTouchTime()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcISensorStorage::preCreate(
		storPath,ARpcISensorStorage::CONTINUOUS,ARpcSensor::SINGLE_GT);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE_GT);
	ARpcContinuousStorage *storage=(ARpcContinuousStorage*)iStorage;
	VERIFY(storage->createAsFixedBlocksDb(ARpcSingleSensorValue(3,false),ARpcISensorStorage::DONT_TOUCH));
	VERIFY(storage->effectiveValuesType()==ARpcSensor::SINGLE_GT);

	//test read/write
	ARpcSingleSensorValue sValGT(3,false);
	sValGT.fromData(testData1);
	sValGT.setTime(10);
	VERIFY(storage->writeSensorValue(&sValGT));
	VERIFY(storage->valuesCount()==1);
	ARpcSingleSensorValue *sValGT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValGT2);
	VERIFY(sValGT2->type()==ARpcSensor::SINGLE_GT);
	VERIFY(sValGT2->dims()==3);
	VERIFY(sValGT2->values()==testData1);
	VERIFY(sValGT2->time()==sValGT.time())
	delete sValGT2;

	//test open existing
	delete storage;
	iStorage=ARpcISensorStorage::open(storPath);
	VERIFY(iStorage);
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
	VERIFY(sValGT2->values()==testData1);
	VERIFY(sValGT2->time()==sValGT.time());
	delete sValGT2;
}

void ARpcContinuousStorageTests::testStorageSingleGTDropTime()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcISensorStorage::preCreate(
		storPath,ARpcISensorStorage::CONTINUOUS,ARpcSensor::SINGLE_GT);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::CONTINUOUS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE_GT);
	ARpcContinuousStorage *storage=(ARpcContinuousStorage*)iStorage;
	VERIFY(storage->createAsFixedBlocksDb(ARpcSingleSensorValue(3,false),ARpcISensorStorage::DROP_TIME));
	VERIFY(storage->effectiveValuesType()==ARpcSensor::SINGLE);

	//test read/write
	ARpcSingleSensorValue sValGT(3,false);
	sValGT.fromData(testData1);
	sValGT.setTime(10);
	VERIFY(storage->writeSensorValue(&sValGT));
	VERIFY(storage->valuesCount()==1);
	ARpcSingleSensorValue *sValNT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==ARpcSensor::SINGLE);
	VERIFY(sValNT2->dims()==3);
	VERIFY(sValNT2->values()==testData1);
	delete sValNT2;

	//test open existing
	delete storage;
	iStorage=ARpcISensorStorage::open(storPath);
	VERIFY(iStorage);
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
	VERIFY(sValNT2->values()==testData1);
	delete sValNT2;
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
