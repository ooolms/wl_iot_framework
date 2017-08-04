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

#include "IotkitAgentSensorDataTranslator.h"
#include "ARpcBase/ARpcAllSensorValues.h"
#include <QUdpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

static const quint16 agentSensorPort=41234;//UDP port, aegnt listen here for measurements
const QString IotkitAgentSensorDataTranslator::type="iotkit-agent";

IotkitAgentSensorDataTranslator::IotkitAgentSensorDataTranslator(const QVariantMap &cfg,QObject *parent)
	:ISensorDataTranslator(cfg,parent)
{
	cLoc=QLocale(QLocale::C);
}

void IotkitAgentSensorDataTranslator::writeSensorValue(ARpcISensorValue *val)
{
	if(config["sensor_name"].toString().isEmpty())return;
	if(!ARpcSensor::isSingle(val->type()))return;
	ARpcSingleSensorValue *sVal=(ARpcSingleSensorValue*)val;
	QString valueStr;
	for(quint32 i=0;i<sVal->dims();++i)
		valueStr+=cLoc.toString(sVal->values()[i])+" ";
	valueStr.chop(1);
	QJsonDocument doc;
	QJsonObject obj;
	obj["n"]=QJsonValue(config["sensor_name"].toString());
	obj["v"]=QJsonValue(valueStr);
	doc.setObject(obj);
	sensorWriteSock.writeDatagram(doc.toJson(QJsonDocument::Compact)+"\n",QHostAddress::LocalHost,agentSensorPort);
}

bool IotkitAgentSensorDataTranslator::checkConfig(const QVariantMap &cfg)
{
	return cfg.contains("sensor_name")&&!cfg["sensor_name"].toString().isEmpty();
}