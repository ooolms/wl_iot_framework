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
#include <QUdpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

static const quint16 agentSensorPort=41234;//UDP port, aegnt listen here for measurements
const QString IotkitAgentSensorDataTranslator::type="iotkit-agent";

IotkitAgentSensorDataTranslator::IotkitAgentSensorDataTranslator(const QVariantMap &cfg,QObject *parent)
	:ISensorDataTranslator(cfg,parent)
{
}

void IotkitAgentSensorDataTranslator::writeSensorValue(ARpcSensorValue *val)
{
	if(config["sensor_name"].toString().isEmpty())return;
	if(val->isEmpty())return;
	QString valueStr;
	for(quint32 i=0;i<val->packetsCount();++i)
		for(quint32 j=0;j<val->type().dim;++j)
			valueStr+=val->valueToString(j,i)+" ";
	valueStr.chop(1);
	QJsonDocument doc;
	QJsonObject obj;
	obj["n"]=QJsonValue(config["sensor_name"].toString());
	obj["v"]=QJsonValue(valueStr);
	doc.setObject(obj);
	qDebug()<<"Write sensor value to iotkit-agent: "<<config["sensor_name"]<<"="<<valueStr;
	sensorWriteSock.writeDatagram(doc.toJson(QJsonDocument::Compact)+"\n",QHostAddress::LocalHost,agentSensorPort);
}

bool IotkitAgentSensorDataTranslator::checkConfig(const QVariantMap &cfg)
{
	return cfg.contains("sensor_name")&&!cfg["sensor_name"].toString().isEmpty();
}
