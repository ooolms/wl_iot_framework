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

#include "ARpcLastNValuesInMemoryStorage.h"

ARpcLastNValuesInMemoryStorage::ARpcLastNValuesInMemoryStorage(const QString &path,const QUuid &devId,
	const QByteArray &devName,const ARpcSensorDef &sensor,TimestampRule tsRule,QObject *parent)
	:ARpcBaseFSSensorStorage(path,devId,devName,sensor,LAST_N_VALUES_IN_MEMORY,tsRule,parent)
{
	opened=false;
}

ARpcLastNValuesInMemoryStorage::~ARpcLastNValuesInMemoryStorage()
{
	close();
	values.clear();
}

bool ARpcLastNValuesInMemoryStorage::create(quint32 storedValuesCount)
{
	if(opened)
		return false;
	if(storedValuesCount==0)
		storedValuesCount=1;
	fsStorageHelper.settings()->setValue("stored_count",QString::number(storedValuesCount));
	fsStorageHelper.settings()->sync();
	if(fsStorageHelper.settings()->status()!=QSettings::NoError)
		return false;
	resizeValues(storedValuesCount);
	opened=true;
	return true;
}

bool ARpcLastNValuesInMemoryStorage::open()
{
	if(opened)
		return false;
	quint32 storedValuesCount=fsStorageHelper.settings()->value("stored_count").toUInt();
	if(storedValuesCount==0)
		storedValuesCount=1;
	resizeValues(storedValuesCount);
	opened=true;
	return true;
}

bool ARpcLastNValuesInMemoryStorage::isOpened() const
{
	return opened;
}

quint64 ARpcLastNValuesInMemoryStorage::valuesCount()
{
	return values.count();
}

ARpcSensorValue* ARpcLastNValuesInMemoryStorage::valueAt(quint64 index)
{
	if(index<(quint32)values.count())
		return hlp.unpackSensorValue(mStoredValuesType,values[index]);
	else
		return 0;
}

bool ARpcLastNValuesInMemoryStorage::writeSensorValue(const ARpcSensorValue *val)
{
	if(!opened)
		return false;
	if(val->type()!=mSensor.type)
		return false;
	QByteArray valData=hlp.packSensorValue(val);
	if(valData.isEmpty())
		return false;
	values.removeAt(0);
	values.append(valData);
	emit newValueWritten(val);
	return true;
}

void ARpcLastNValuesInMemoryStorage::close()
{
	if(!opened)
		return;
	values.clear();
	opened=false;
}

void ARpcLastNValuesInMemoryStorage::resizeValues(quint32 sz)
{
	values.clear();
	QScopedPointer<ARpcSensorValue> templateValue(ARpcSensorValue::createSensorValue(mStoredValuesType));
	QByteArray templateData=hlp.packSensorValue(templateValue.data());
	for(quint32 i=0;i<sz;++i)
		values.append(templateData);
}
