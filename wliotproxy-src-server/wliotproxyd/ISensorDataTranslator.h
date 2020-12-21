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
	explicit ISensorDataTranslator(const QUuid &devId,const QByteArray &devName,const WLIOT::SensorDef &sens,
		const WLIOT::ISensorStorage::DataExportConfig &cfg,QObject *parent=0);
	virtual ~ISensorDataTranslator(){}
	virtual void writeSensorValue(WLIOT::SensorValue *val)=0;
	virtual bool checkConfig(WLIOT::ISensorStorage::DataExportConfig &cfg)=0;
	virtual QByteArray name()const=0;
	virtual QUuid uid()const=0;
	static QList<QUuid> availableTranslators();
	static void translatorConfig(const QUuid &uid,QByteArray &name,QByteArrayList &params);
	static QUuid findTranslator(const QByteArray &nameOrId);
	static bool hasTranslator(const QUuid &uid);

	static ISensorDataTranslator* makeTranslator(const QByteArray &nameOrId,const QUuid &devId,
		const QByteArray &devName,const WLIOT::SensorDef &sens,const WLIOT::ISensorStorage::DataExportConfig &cfg);
	static ISensorDataTranslator* makeTranslator(const QUuid &uid,const QUuid &devId,
		const QByteArray &devName,const WLIOT::SensorDef &sens,const WLIOT::ISensorStorage::DataExportConfig &cfg);

protected:
	WLIOT::ISensorStorage::DataExportConfig config;
	WLIOT::SensorDef sensor;
	QUuid deviceId;
	QByteArray deviceName;
};

#endif // ISENSORDATATRANSLATOR_H
