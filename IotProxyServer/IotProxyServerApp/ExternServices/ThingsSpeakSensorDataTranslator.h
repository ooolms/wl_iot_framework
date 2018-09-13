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

#ifndef THINGSSPEAKSENSORDATATRANSLATOR_H
#define THINGSSPEAKSENSORDATATRANSLATOR_H

#include <QObject>
#include "../ISensorDataTranslator.h"
#include <QNetworkAccessManager>

class ThingsSpeakSensorDataTranslator
	:public ISensorDataTranslator
{
	Q_OBJECT
public:
	explicit ThingsSpeakSensorDataTranslator(const QUuid &devId,const ARpcSensorDef &sens,
	const ARpcISensorStorage::DataExportConfig &cfg,QObject *parent=0);
	virtual void writeSensorValue(ARpcSensorValue *val) override;
	virtual bool checkConfig(const ARpcISensorStorage::DataExportConfig &cfg) override;
	virtual QByteArray type()const override;

public:
	static const QByteArray mType;

private:
	QNetworkAccessManager mgr;
};

#endif // THINGSSPEAKSENSORDATATRANSLATOR_H
