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

#include "ISensorDataTranslator.h"
#include "ExternServices/IotkitAgentSensorDataTranslator.h"
#include "ExternServices/AlterozoomSensorDataTranslator.h"
#include "ExternServices/ThingsSpeakSensorDataTranslator.h"
#include "ExternServices/AlterozoomAuthentificationStorage.h"

ISensorDataTranslator::ISensorDataTranslator(const QUuid &devId,const QByteArray &devName,const SensorDef &sens,
	const ISensorStorage::DataExportConfig &cfg,QObject *parent)
	:QObject(parent)
{
	deviceId=devId;
	deviceName=devName;
	sensor=sens;
	config=cfg;
}

QList<QUuid> ISensorDataTranslator::availableTranslators()
{
	return QList<QUuid>()<<AlterozoomSensorDataTranslator::mUid<<IotkitAgentSensorDataTranslator::mUid<<
		ThingsSpeakSensorDataTranslator::mUid;
}

void ISensorDataTranslator::translatorConfig(const QUuid &uid,QByteArray &name,QByteArrayList &params)
{
	name.clear();
	params.clear();
	if(uid==IotkitAgentSensorDataTranslator::mUid)
	{
		name=IotkitAgentSensorDataTranslator::mName;
		params=IotkitAgentSensorDataTranslator::mParams;
	}
	else if(uid==AlterozoomSensorDataTranslator::mUid)
	{
		name=AlterozoomSensorDataTranslator::mName;
		params=AlterozoomSensorDataTranslator::mParams;
	}
	else if(uid==ThingsSpeakSensorDataTranslator::mUid)
	{
		name=ThingsSpeakSensorDataTranslator::mName;
		params=ThingsSpeakSensorDataTranslator::mParams;
	}
}

QUuid ISensorDataTranslator::findTranslator(const QByteArray &nameOrId)
{
	QUuid uid=QUuid(nameOrId);
	if(uid.isNull())
	{
		if(nameOrId==IotkitAgentSensorDataTranslator::mName)
			return IotkitAgentSensorDataTranslator::mUid;
		else if(nameOrId==AlterozoomSensorDataTranslator::mName)
			return AlterozoomSensorDataTranslator::mUid;
		else if(nameOrId==ThingsSpeakSensorDataTranslator::mName)
			return ThingsSpeakSensorDataTranslator::mUid;
		return QUuid();
	}
	if(uid==IotkitAgentSensorDataTranslator::mUid||uid==AlterozoomSensorDataTranslator::mUid||
		uid==ThingsSpeakSensorDataTranslator::mUid)
		return uid;
	return QUuid();
}

bool ISensorDataTranslator::hasTranslator(const QUuid &uid)
{
	return uid==IotkitAgentSensorDataTranslator::mUid||uid==AlterozoomSensorDataTranslator::mUid||
		uid==ThingsSpeakSensorDataTranslator::mUid;
}

ISensorDataTranslator* ISensorDataTranslator::makeTranslator(const QByteArray &nameOrId,const QUuid &devId,
	const QByteArray &devName,const SensorDef &sens,const ISensorStorage::DataExportConfig &cfg)
{
	QUuid uid=findTranslator(nameOrId);
	if(uid.isNull())return 0;
	return makeTranslator(uid,devId,devName,sens,cfg);
}

ISensorDataTranslator *ISensorDataTranslator::makeTranslator(const QUuid &uid,const QUuid &devId,
	const QByteArray &devName,const SensorDef &sens,const ISensorStorage::DataExportConfig &cfg)
{
	if(uid==IotkitAgentSensorDataTranslator::mUid)
		return new IotkitAgentSensorDataTranslator(devId,devName,sens,cfg);
	else if(uid==AlterozoomSensorDataTranslator::mUid)
		return new AlterozoomSensorDataTranslator(devId,devName,sens,cfg);
	else if(uid==ThingsSpeakSensorDataTranslator::mUid)
		return new ThingsSpeakSensorDataTranslator(devId,devName,sens,cfg);
	return 0;
}
