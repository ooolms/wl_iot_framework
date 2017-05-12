#include "ARpcLastNValuesStorageTests.h"
#include "RemoveDirRecusive.h"
#include "ARpcLocalStorage/ARpcLastNValuesStorage.h"
#include "ARpcBase/ARpcSingleSensorValue.h"
#include "ARpcBase/ARpcPacketSensorValue.h"
#include "ARpcBase/ARpcTextSensorValue.h"
#include <QDateTime>

static const QString storPath=QString("/tmp/ARpcTestStorage");
static const QVector<double> testData1=QVector<double>()<<12.0<<621.124<<2845.125626;
static const QVector<double> testData2=QVector<double>()<<156.6<<124.1<<0.1;

ARpcLastNValuesStorageTests::ARpcLastNValuesStorageTests(QObject *parent)
	:QtUnitTestSet("ARpcLastNValuesStorageTests",parent)
{
	addTest((TestFunction)&ARpcLastNValuesStorageTests::testStorageSingleDontTouchTime,
		"Test storage for SINGLE values with time policy DONT_TOUCH");
	addTest((TestFunction)&ARpcLastNValuesStorageTests::testStorageSingleLTDontTouchTime,
		"Test storage for SINGLE_LT values with time policy DONT_TOUCH");
	addTest((TestFunction)&ARpcLastNValuesStorageTests::testStorageSingleGTDropTime,
		"Test storage for SINGLE_GT values with time policy DROP_TIME");
}

void ARpcLastNValuesStorageTests::testStorageSingleDontTouchTime()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcISensorStorage::preCreate(
		storPath,ARpcISensorStorage::LAST_N_VALUES,ARpcSensor::SINGLE);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::LAST_N_VALUES);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE);
	ARpcLastNValuesStorage *storage=(ARpcLastNValuesStorage*)iStorage;
	VERIFY(storage->create(3,ARpcSingleSensorValue(3),ARpcISensorStorage::DONT_TOUCH));
	VERIFY(storage->effectiveValuesType()==ARpcSensor::SINGLE);

	//test read/write
	ARpcSingleSensorValue sValNT(3);
	sValNT.fromData(testData1);
	VERIFY(storage->writeSensorValue(&sValNT));
	VERIFY(storage->valuesCount()==3);
	ARpcSingleSensorValue *sValNT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==ARpcSensor::SINGLE)
	VERIFY(sValNT2->dims()==3);
	VERIFY(sValNT2->values()==testData1);
	delete sValNT2;

	//write second value
	sValNT.fromData(testData2);
	VERIFY(storage->writeSensorValue(&sValNT));
	VERIFY(storage->valuesCount()==3);
	sValNT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==ARpcSensor::SINGLE)
	VERIFY(sValNT2->dims()==3);
	VERIFY(sValNT2->values()==testData2);
	delete sValNT2;
	sValNT2=(ARpcSingleSensorValue*)storage->valueAt(1);
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
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::LAST_N_VALUES);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE);
	VERIFY(iStorage->effectiveValuesType()==ARpcSensor::SINGLE);
	storage=(ARpcLastNValuesStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==3);
	sValNT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==ARpcSensor::SINGLE)
	VERIFY(sValNT2->dims()==3);
	VERIFY(sValNT2->values()==testData2);
	delete sValNT2;
	sValNT2=(ARpcSingleSensorValue*)storage->valueAt(1);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==ARpcSensor::SINGLE)
	VERIFY(sValNT2->dims()==3);
	VERIFY(sValNT2->values()==testData1);
	delete sValNT2;
}

void ARpcLastNValuesStorageTests::testStorageSingleLTDontTouchTime()
{
	//continuous storage replaces LT by GT in any case
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));
	qint64 currentDT=QDateTime::currentMSecsSinceEpoch();

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcISensorStorage::preCreate(
		storPath,ARpcISensorStorage::LAST_N_VALUES,ARpcSensor::SINGLE_LT);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::LAST_N_VALUES);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE_LT);
	ARpcLastNValuesStorage *storage=(ARpcLastNValuesStorage*)iStorage;
	VERIFY(storage->create(1,ARpcSingleSensorValue(3,true),ARpcISensorStorage::DONT_TOUCH));
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

	//write second value
	sValLT.fromData(testData2);
	VERIFY(storage->writeSensorValue(&sValLT));
	VERIFY(storage->valuesCount()==1);
	sValGT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValGT2);
	VERIFY(sValGT2->type()==ARpcSensor::SINGLE_GT);
	VERIFY(sValGT2->dims()==3);
	VERIFY(sValGT2->values()==testData2);
	VERIFY(sValGT2->time()>=currentDT);
	delete sValGT2;

	//test open existing
	delete storage;
	iStorage=ARpcISensorStorage::preOpen(storPath);
	VERIFY(iStorage);
	VERIFY(iStorage->open());
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::LAST_N_VALUES);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE_LT);
	VERIFY(iStorage->effectiveValuesType()==ARpcSensor::SINGLE_GT);
	storage=(ARpcLastNValuesStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==1);
	sValGT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValGT2);
	VERIFY(sValGT2->type()==ARpcSensor::SINGLE_GT);
	VERIFY(sValGT2->dims()==3);
	VERIFY(sValGT2->values()==testData2);
	VERIFY(sValGT2->time()>=currentDT);
	delete sValGT2;
}

void ARpcLastNValuesStorageTests::testStorageSingleGTDropTime()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcISensorStorage::preCreate(
		storPath,ARpcISensorStorage::LAST_N_VALUES,ARpcSensor::SINGLE_GT);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::LAST_N_VALUES);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE_GT);
	ARpcLastNValuesStorage *storage=(ARpcLastNValuesStorage*)iStorage;
	VERIFY(storage->create(3,ARpcSingleSensorValue(3,false),ARpcISensorStorage::DROP_TIME));
	VERIFY(storage->effectiveValuesType()==ARpcSensor::SINGLE);

	//test read/write
	ARpcSingleSensorValue sValGT(3,false);
	sValGT.fromData(testData1);
	sValGT.setTime(10);
	VERIFY(storage->writeSensorValue(&sValGT));
	VERIFY(storage->valuesCount()==3);
	ARpcSingleSensorValue *sValNT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==ARpcSensor::SINGLE);
	VERIFY(sValNT2->dims()==3);
	VERIFY(sValNT2->values()==testData1);
	delete sValNT2;

	//test open existing
	delete storage;
	iStorage=ARpcISensorStorage::preOpen(storPath);
	VERIFY(iStorage);
	VERIFY(iStorage->open());
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::LAST_N_VALUES);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE_GT);
	VERIFY(iStorage->effectiveValuesType()==ARpcSensor::SINGLE);
	storage=(ARpcLastNValuesStorage*)iStorage;

	//test read value
	VERIFY(storage->valuesCount()==3);
	sValNT2=(ARpcSingleSensorValue*)storage->valueAt(0);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==ARpcSensor::SINGLE);
	VERIFY(sValNT2->dims()==3);
	VERIFY(sValNT2->values()==testData1);
	delete sValNT2;
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
