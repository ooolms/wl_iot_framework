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

#include "ThingsSpeakSensorDataTranslator.h"
#include <QDebug>
#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>

const QByteArray ThingsSpeakSensorDataTranslator::mType="thingspeak";

ThingsSpeakSensorDataTranslator::ThingsSpeakSensorDataTranslator(
	const QUuid &devId,const ARpcSensorDef &sens,const ARpcISensorStorage::DataExportConfig &cfg,QObject *parent)
	:ISensorDataTranslator(devId,sens,cfg,parent)
{
}

void ThingsSpeakSensorDataTranslator::setProxy(const QNetworkProxy &proxy)
{
	mgr.setProxy(proxy);
}

void ThingsSpeakSensorDataTranslator::writeSensorValue(ARpcSensorValue *val)
{
	if(val->type().packType!=ARpcSensorDef::SINGLE)return;
	QByteArray apiKey=config["api_key"];
	QByteArrayList fieldValues;
	if(val->type().tsType!=ARpcSensorDef::NO_TIME)
		fieldValues.append(QByteArray::number(val->time()));
	for(quint32 i=0;i<val->type().dim;++i)
	{
		if(val->type().numType==ARpcSensorDef::TEXT)
			fieldValues.append(((ARpcSensorValueText*)val)->get(i));
		else fieldValues.append(QByteArray::number(val->valueToDouble(i)));
	}
	if(fieldValues.count()>8)return;
	qDebug()<<"VALUE EXPORTED THINGSPEAK: "<<apiKey<<":"<<deviceId<<":"<<sensor.name<<":"<<fieldValues;
	QUrlQuery q;
	q.addQueryItem("api_key",QString::fromUtf8(apiKey));
	for(int i=0;i<fieldValues.count();++i)
		q.addQueryItem("field"+QString::number(i+1),QString::fromUtf8(fieldValues[i]));
	QNetworkRequest rq(QUrl("https://api.thingspeak.com/update"));
	rq.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
	QNetworkReply *reply=mgr.post(rq,q.query().toUtf8());
	QEventLoop loop;
	connect(reply,&QNetworkReply::finished,&loop,&QEventLoop::quit);
	loop.exec(QEventLoop::ExcludeUserInputEvents);
	reply->deleteLater();
}

bool ThingsSpeakSensorDataTranslator::checkConfig(const ARpcISensorStorage::DataExportConfig &cfg)
{
	return cfg.contains("api_key")&&!cfg["api_key"].isEmpty();
}

QByteArray ThingsSpeakSensorDataTranslator::type()const
{
	return mType;
}
