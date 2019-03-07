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

#ifndef ISENSORDATATRANSLATOR_H
#define ISENSORDATATRANSLATOR_H

#include <QObject>
#include <QVariant>
#include "wliot/devices/SensorValue.h"
#include "wliot/storages/BaseFSSensorStorage.h"
#include <QUuid>

class ISensorDataTranslator
	:public QObject
{
	Q_OBJECT
public:
	explicit ISensorDataTranslator(const QUuid &devId,const SensorDef &sens,
		const ISensorStorage::DataExportConfig &cfg,QObject *parent=0);
	virtual ~ISensorDataTranslator(){}
	virtual void writeSensorValue(SensorValue *val)=0;
	virtual bool checkConfig(ISensorStorage::DataExportConfig &cfg)=0;
	virtual QByteArray type()const=0;
	static QByteArrayList availableTranslators();

	static ISensorDataTranslator* makeTranslator(const QByteArray &type,const QUuid &devId,const SensorDef &sens,
		const ISensorStorage::DataExportConfig &cfg);

protected:
	ISensorStorage::DataExportConfig config;
	SensorDef sensor;
	QUuid deviceId;
};

#endif // ISENSORDATATRANSLATOR_H
