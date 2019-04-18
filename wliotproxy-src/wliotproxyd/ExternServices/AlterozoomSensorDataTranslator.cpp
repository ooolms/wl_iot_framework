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

#include "AlterozoomSensorDataTranslator.h"
#include "AlterozoomAuthentificationStorage.h"
#include <QDebug>

const QByteArray AlterozoomSensorDataTranslator::mType="alterozoom";

AlterozoomSensorDataTranslator::AlterozoomSensorDataTranslator(
	const QUuid &devId,const QByteArray &devName,const SensorDef &sens,
	const ISensorStorage::DataExportConfig &cfg,QObject *parent)
	:ISensorDataTranslator(devId,devName,sens,cfg,parent)
{
	host=AlterozoomAuthentificationStorage::getDefaultHost();
	if(config.contains("host")&&!config["host"].isEmpty())
		host=config["host"];
	email=config["email"];
	AlterozoomAuthKey k={host,email};
	if(!AlterozoomAuthentificationStorage::getAuthMap().contains(k))
	{
		qWarning()<<"No alterozoom token found for data posting: "<<host<<":"<<email;
		ready=false;
	}
	else
	{
		AlterozoomAuthValue v=AlterozoomAuthentificationStorage::getAuthMap()[k];
		api.setStoredUser(host,v.token,email);
	}
	connect(&api,&AlterozoomApi::authenticationComplete,this,&AlterozoomSensorDataTranslator::onAuthenticationComplete);
	connect(&api,&AlterozoomApi::sensorCreated,this,&AlterozoomSensorDataTranslator::onSensorCreated);
}

void AlterozoomSensorDataTranslator::writeSensorValue(SensorValue *val)
{
	if(!ready||val->type()!=sensor.type)return;
	qDebug()<<"VALUE EXPORTED ALTEROZOOM: "<<host<<":"<<email<<":"<<
		deviceId<<":"<<sensor.name<<":"<<val->dumpToMsgArgs();
	api.postMeasurement(host,email,deviceId,sensor.name,val);
}

bool AlterozoomSensorDataTranslator::checkConfig(ISensorStorage::DataExportConfig &cfg)
{
	if(!cfg.contains("host"))
		cfg["host"]=AlterozoomAuthentificationStorage::getDefaultHost();
	return !cfg.value("email").isEmpty();
}

QByteArray AlterozoomSensorDataTranslator::type()const
{
	return mType;
}

void AlterozoomSensorDataTranslator::onAuthenticationComplete(bool ok,const QByteArray &host,const QByteArray &email)
{
	if(ok&&host==this->host&&email==this->email)
		api.createSensor(host,email,deviceId,deviceName,sensor);
}

void AlterozoomSensorDataTranslator::onSensorCreated(bool ok,const QByteArray &host,const QByteArray &email,
	const QUuid &devId,const QByteArray &sensorName)
{
	if(ok&&host==this->host&&email==this->email&&deviceId==devId&&sensor.name==sensorName)
		ready=true;
}
