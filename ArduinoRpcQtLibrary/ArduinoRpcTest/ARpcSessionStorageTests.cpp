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
		storPath,ARpcISensorStorage::AUTO_SESSIONS,ARpcSensor::SINGLE);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::AUTO_SESSIONS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE);
	ARpcSessionStorage *storage=(ARpcSessionStorage*)iStorage;
	VERIFY(storage->createAsChainedBlocksDb(ARpcISensorStorage::DONT_TOUCH));
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
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE);
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
		storPath,ARpcISensorStorage::MANUAL_SESSIONS,ARpcSensor::SINGLE_LT);
	VERIFY(iStorage->getStoreMode()==ARpcISensorStorage::MANUAL_SESSIONS);
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE_LT);
	ARpcSessionStorage *storage=(ARpcSessionStorage*)iStorage;
	VERIFY(storage->createAsFixedBlocksDb(ARpcSingleSensorValue(3,true),ARpcISensorStorage::DONT_TOUCH));
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
	VERIFY(iStorage->sensorValuesType()==ARpcSensor::SINGLE_LT);
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
