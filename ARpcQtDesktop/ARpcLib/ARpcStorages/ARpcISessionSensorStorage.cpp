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

#include "ARpcISessionSensorStorage.h"

ARpcISessionSensorStorage::ARpcISessionSensorStorage(
	const QUuid &devId,const QByteArray &devName,ARpcSensorDef sensor,ARpcISensorStorage::StoreMode stMode,
	ARpcISensorStorage::TimestampRule tsRule,ARpcSensorDef::Type storedValType,QObject *parent)
	:ARpcISensorStorage(devId,devName,sensor,stMode,tsRule,storedValType,parent)
{
}

bool ARpcISessionSensorStorage::values(const QUuid &sessionId,quint64 index,quint64 count,quint64 step,
	VeryBigArray<ARpcSensorValue*> &vals)
{
	if(!isOpened())
		return false;
	vals.clear();
	for(quint64 i=0;i<count;++i)
	{
		ARpcSensorValue *v=valueAt(sessionId,index+i*step);
		if(v)vals.append(v);
		else break;
	}
	return true;
}

quint64 ARpcISessionSensorStorage::findInGTIndex(const QUuid &sessionId,qint64 ts)
{
	Q_UNUSED(sessionId)
	Q_UNUSED(ts)
	return 0;
}
