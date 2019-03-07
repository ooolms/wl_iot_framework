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

#ifndef IOTKITAGENTSENSORDATATRANSLATOR_H
#define IOTKITAGENTSENSORDATATRANSLATOR_H

#include "../ISensorDataTranslator.h"
#include <QUdpSocket>

/*
 * config:
 * sensor_name: iotkit-agent sensor name
*/
class IotkitAgentSensorDataTranslator
	:public ISensorDataTranslator
{
	Q_OBJECT
public:
	explicit IotkitAgentSensorDataTranslator(
		const QUuid &devId,const SensorDef &sens,const ISensorStorage::DataExportConfig &cfg,QObject *parent=0);
	virtual void writeSensorValue(SensorValue *val)override;
	virtual bool checkConfig(ISensorStorage::DataExportConfig &cfg)override;
	virtual QByteArray type()const override;

public:
	static const QByteArray mType;

private:
	QUdpSocket sensorWriteSock;//sockets for iotkit-agent
};

#endif // IOTKITAGENTSENSORDATATRANSLATOR_H
