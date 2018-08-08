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

#include "ARpcLastValueInMemoryStorage.h"

ARpcLastValueInMemoryStorage::ARpcLastValueInMemoryStorage(const ARpcSensorDef &sensor,const QUuid &devId,
	const QByteArray &devName,QObject *parent)
	:ARpcISensorStorage(sensor,devId,devName,parent)
{
	opened=false;
}

ARpcLastValueInMemoryStorage::~ARpcLastValueInMemoryStorage()
{
	close();
	value.clear();
}

bool ARpcLastValueInMemoryStorage::open()
{
	if(opened)
		return false;
	hlp=ARpcDBDriverHelpers(timestampRule);
	opened=true;
	return true;
}

bool ARpcLastValueInMemoryStorage::isOpened() const
{
	return opened;
}

quint64 ARpcLastValueInMemoryStorage::valuesCount()
{
	if(!value.isEmpty())
		return 1;
	else
		return 0;
}

ARpcSensorValue *ARpcLastValueInMemoryStorage::valueAt(quint64 index)
{
	if(!value.isEmpty()&&index==0)
		return hlp.unpackSensorValue(effectiveValType,value);
	else
		return 0;
}

ARpcISensorStorage::StoreMode ARpcLastValueInMemoryStorage::getStoreMode() const
{
	return LAST_VALUE_IN_MEMORY;
}

bool ARpcLastValueInMemoryStorage::writeSensorValue(const ARpcSensorValue *val)
{
	if(!opened)
		return false;
	if(val->type()!=mSensor.type)
		return false;
	value=hlp.packSensorValue(val);
	if(value.isEmpty())
		return false;
	emit newValueWritten(val);
	return true;
}

void ARpcLastValueInMemoryStorage::closeInternal()
{
	if(!opened)
		return;
	value.clear();
	opened=false;
}
