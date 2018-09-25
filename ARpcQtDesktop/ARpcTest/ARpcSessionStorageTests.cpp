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
#include "ARpcStorages/ARpcSessionStorage.h"
#include "TestData.h"
#include <QDateTime>

//CRIT test muliple sessions

ARpcSessionStorageTests::ARpcSessionStorageTests(QObject *parent)
	:QtUnitTestSet("ARpcSessionStorageTests",parent)
{
	addTest((TestFunction)&ARpcSessionStorageTests::testStorageSingleDontTouchTime,
		"Test storage for SINGLE values with time policy DONT_TOUCH");
}

void ARpcSessionStorageTests::testStorageSingleDontTouchTime()
{
	RemoveDirRecusive::rmDirContentsRec(QDir(storPath));

	//test creation as fixed blocks storage
	ARpcISensorStorage *iStorage=ARpcFSSensorStorageHelper::preCreate(
		storPath,deviceId,deviceName,singleNT,ARpcISensorStorage::AUTO_SESSIONS,ARpcISensorStorage::DONT_TOUCH);
	VERIFY(iStorage->storeMode()==ARpcISensorStorage::AUTO_SESSIONS);
	VERIFY(iStorage->sensor()==singleNT);
	ARpcSessionStorage *storage=(ARpcSessionStorage*)iStorage;
	VERIFY(storage->create());
	VERIFY(storage->storedValuesType()==singleNT.type);

	//create session
	QUuid sId;
	VERIFY(storage->createSession("Session1",sId));
	VERIFY(storage->openMainWriteSession(sId))

	//test read/write
	ARpcSensorValueF32 sValNT(singleNT.type);
	sValNT.parseBinary(singleData1Binary);
	VERIFY(storage->writeSensorValue(&sValNT));
	VERIFY(storage->valuesCount(storage->getMainWriteSessionId())==1);
	ARpcSensorValue *sValNT2=storage->valueAt(storage->getMainWriteSessionId(),0);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==singleNT.type)
	VERIFY(((ARpcSensorValueF32*)sValNT2)->getSample()==singleData1);
	delete sValNT2;

	//test open existing
	delete storage;
	iStorage=ARpcFSSensorStorageHelper::preOpen(storPath);
	VERIFY(iStorage);
	VERIFY(iStorage->open());
	VERIFY(iStorage->storeMode()==ARpcISensorStorage::AUTO_SESSIONS);
	VERIFY(iStorage->sensor()==singleNT);
	VERIFY(iStorage->storedValuesType()==singleNT.type);
	storage=(ARpcSessionStorage*)iStorage;

	//test open session
	QList<QUuid> ids;
	QByteArrayList titles;
	VERIFY(storage->listSessions(ids,titles));
	VERIFY(ids.count()==1&&titles.count()==1);
	VERIFY(ids[0]==sId&&titles[0]=="Session1");
	VERIFY(storage->openMainWriteSession(sId));

	//test read value
	VERIFY(storage->valuesCount(storage->getMainWriteSessionId())==1);
	sValNT2=storage->valueAt(storage->getMainWriteSessionId(),0);
	VERIFY(sValNT2);
	VERIFY(sValNT2->type()==singleNT.type)
	VERIFY(((ARpcSensorValueF32*)sValNT2)->getSample()==singleData1);
	delete sValNT2;
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
