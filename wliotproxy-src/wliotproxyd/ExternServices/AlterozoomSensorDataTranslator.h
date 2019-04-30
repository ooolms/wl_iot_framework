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

#ifndef ALTEROZOOMSENSORDATATRANSLATOR_H
#define ALTEROZOOMSENSORDATATRANSLATOR_H

#include <QObject>
#include "../ISensorDataTranslator.h"
#include "AlterozoomApi.h"

class AlterozoomSensorDataTranslator
	:public ISensorDataTranslator
{
	Q_OBJECT
public:
	explicit AlterozoomSensorDataTranslator(
		const QUuid &devId,const QByteArray &devName,const SensorDef &sens,
		const ISensorStorage::DataExportConfig &cfg,QObject *parent=nullptr);

public:
	virtual void writeSensorValue(SensorValue *val)override;
	virtual bool checkConfig(ISensorStorage::DataExportConfig &cfg)override;
	virtual QByteArray name()const override;
	virtual QUuid uid()const override;

public:
	static const QByteArray mName;
	static const QUuid mUid;
	static const QByteArrayList mParams;

private slots:
	void onAuthenticationComplete(bool ok,const QByteArray &host,const QByteArray &email);
	void onSensorCreated(bool ok,const QByteArray &host,const QByteArray &email,
		const QUuid &devId,const QByteArray &sensorName);

private:
	bool ready;
	AlterozoomApi api;
	QByteArray host,email;
};

#endif // ALTEROZOOMSENSORDATATRANSLATOR_H
