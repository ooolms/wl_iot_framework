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
#include "../ServerLogs.h"
#include <QDebug>
#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>

using namespace WLIOT;

const QByteArray ThingsSpeakSensorDataTranslator::mName="thingspeak";
const QUuid ThingsSpeakSensorDataTranslator::mUid=QUuid("{de16460a-1d2a-4096-b664-bed96f0c6e9a}");
const QByteArrayList ThingsSpeakSensorDataTranslator::mParams=QByteArrayList()<<"api_key";

ThingsSpeakSensorDataTranslator::ThingsSpeakSensorDataTranslator(
	const QUuid &devId,const QByteArray &devName,const SensorDef &sens,
	const ISensorStorage::DataExportConfig &cfg,QObject *parent)
	:ISensorDataTranslator(devId,devName,sens,cfg,parent)
{
}

void ThingsSpeakSensorDataTranslator::setProxy(const QNetworkProxy &proxy)
{
	mgr.setProxy(proxy);
}

void ThingsSpeakSensorDataTranslator::writeSensorValue(SensorValue *val)
{
	if(val->type().packType!=SensorDef::SINGLE)return;
	QByteArray apiKey=config["api_key"];
	QByteArrayList fieldValues;
	if(val->type().tsType!=SensorDef::NO_TIME)
		fieldValues.append(QByteArray::number(val->time()));
	for(quint32 i=0;i<val->type().dim;++i)
	{
		if(val->type().numType==SensorDef::TEXT)
			fieldValues.append(((SensorValueText*)val)->getT(i));
		else fieldValues.append(QByteArray::number(val->valueToDouble(i),'g',200));
	}
	if(fieldValues.count()>8)return;
	ServerLogs::logDevices(QtDebugMsg,"VALUE EXPORTED THINGSPEAK: "+apiKey+":"+deviceId.toByteArray()+":"+sensor.name+
		":"+fieldValues.join('|'));
	QUrlQuery q;
	q.addQueryItem("api_key",QString::fromUtf8(apiKey));
	for(int i=0;i<fieldValues.count();++i)
		q.addQueryItem("field"+QString::fromUtf8(QByteArray::number(i+1)),QString::fromUtf8(fieldValues[i]));
	QNetworkRequest rq(QUrl("https://api.thingspeak.com/update"));
	rq.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
	QNetworkReply *reply=mgr.post(rq,q.query().toUtf8());
	connect(this,&ThingsSpeakSensorDataTranslator::destroyed,reply,&QNetworkReply::abort);
	connect(reply,&QNetworkReply::finished,reply,&QNetworkReply::deleteLater);
}

bool ThingsSpeakSensorDataTranslator::checkConfig(ISensorStorage::DataExportConfig &cfg)
{
	return !cfg.value("api_key").isEmpty();
}

QByteArray ThingsSpeakSensorDataTranslator::name()const
{
	return mName;
}

QUuid ThingsSpeakSensorDataTranslator::uid() const
{
	return mUid;
}
