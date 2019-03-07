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

#include "wliot/storages/LastNValuesInMemoryStorage.h"
#include "FSSensorStorageHelper.h"
#include "DBDriverHelpers.h"

LastNValuesInMemoryStorage::LastNValuesInMemoryStorage(const QString &path,const QUuid &devId,
	const QByteArray &devName,const SensorDef &sensor,TimestampRule tsRule,QObject *parent)
	:BaseFSSensorStorage(path,devId,devName,sensor,LAST_N_VALUES_IN_MEMORY,tsRule,parent)
{
	opened=false;
}

LastNValuesInMemoryStorage::~LastNValuesInMemoryStorage()
{
	close();
	values.clear();
}

bool LastNValuesInMemoryStorage::create(quint32 storedValuesCount)
{
	if(opened)
		return false;
	if(storedValuesCount==0)
		storedValuesCount=1;
	fsStorageHelper->settings()->setValue("stored_count",QString::number(storedValuesCount));
	fsStorageHelper->settings()->sync();
	if(fsStorageHelper->settings()->status()!=QSettings::NoError)
		return false;
	resizeValues(storedValuesCount);
	opened=true;
	return true;
}

bool LastNValuesInMemoryStorage::open()
{
	if(opened)
		return false;
	quint32 storedValuesCount=fsStorageHelper->settings()->value("stored_count").toUInt();
	if(storedValuesCount==0)
		storedValuesCount=1;
	resizeValues(storedValuesCount);
	opened=true;
	return true;
}

bool LastNValuesInMemoryStorage::isOpened() const
{
	return opened;
}

quint64 LastNValuesInMemoryStorage::valuesCount()
{
	return values.count();
}

SensorValue* LastNValuesInMemoryStorage::valueAt(quint64 index)
{
	if(index<(quint32)values.count())
		return hlp->unpackSensorValue(mStoredValuesType,values[index]);
	else
		return 0;
}

bool LastNValuesInMemoryStorage::writeSensorValue(const SensorValue *val)
{
	if(!opened)
		return false;
	if(val->type()!=mSensor.type)
		return false;
	QByteArray valData=hlp->packSensorValue(val);
	if(valData.isEmpty())
		return false;
	values.removeAt(0);
	values.append(valData);
	QScopedPointer<SensorValue> valWritten(hlp->unpackSensorValue(mStoredValuesType,valData));
	if(valWritten.data())
		emit newValueWritten(valWritten.data());
	return true;
}

void LastNValuesInMemoryStorage::close()
{
	if(!opened)
		return;
	values.clear();
	opened=false;
}

void LastNValuesInMemoryStorage::resizeValues(quint32 sz)
{
	values.clear();
	QScopedPointer<SensorValue> templateValue(SensorValue::createSensorValue(mStoredValuesType));
	QByteArray templateData=hlp->packSensorValue(templateValue.data());
	for(quint32 i=0;i<sz;++i)
		values.append(templateData);
}
