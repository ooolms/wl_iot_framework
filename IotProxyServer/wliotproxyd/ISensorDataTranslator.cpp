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

ISensorDataTranslator::ISensorDataTranslator(const QUuid &devId,const ARpcSensorDef &sens,
	const ARpcISensorStorage::DataExportConfig &cfg,QObject *parent)
	:QObject(parent)
{
	deviceId=devId;
	sensor=sens;
	config=cfg;
}

QByteArrayList ISensorDataTranslator::availableTranslators()
{
	return QByteArrayList()<<IotkitAgentSensorDataTranslator::mType<<AlterozoomSensorDataTranslator::mType<<
		ThingsSpeakSensorDataTranslator::mType;
}

ISensorDataTranslator* ISensorDataTranslator::makeTranslator(
	const QByteArray &type,const QUuid &devId,const ARpcSensorDef &sens,
	const ARpcISensorStorage::DataExportConfig &cfg)
{
	if(type==IotkitAgentSensorDataTranslator::mType)
		return new IotkitAgentSensorDataTranslator(devId,sens,cfg);
	else if(type==AlterozoomSensorDataTranslator::mType)
		return new AlterozoomSensorDataTranslator(devId,sens,cfg);
	else if(type==ThingsSpeakSensorDataTranslator::mType)
		return new ThingsSpeakSensorDataTranslator(devId,sens,cfg);
	return 0;
}
