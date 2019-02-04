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
	const QUuid &devId,const ARpcSensorDef &sens,const ARpcISensorStorage::DataExportConfig &cfg,QObject *parent)
	:ISensorDataTranslator(devId,sens,cfg,parent)
{
	host=AlterozoomAuthentificationStorage::getDefaultHost();
	if(config.contains("host")&&!config["host"].isEmpty())
		host=config["host"];
	email=config["email"];
	AlterozoomAuthentificationStorage::AuthKey k={host,email};
	if(!AlterozoomAuthentificationStorage::getAuthMap().contains(k))
	{
		qWarning()<<"No alterozoom token found for data posting: "<<host<<":"<<email;
		ready=false;
	}
	else
	{
		AlterozoomAuthentificationStorage::AuthValue v=AlterozoomAuthentificationStorage::getAuthMap()[k];
		ready=api.setStoredUser(host,v.token,email,v.userId);
		if(!ready)return;
		bool wc;
		ready=api.createSensor(host,email,deviceId,"",sensor,wc);
	}
}

void AlterozoomSensorDataTranslator::writeSensorValue(ARpcSensorValue *val)
{
	if(!ready||val->type()!=sensor.type)return;
	api.postMeasurement(host,email,deviceId,sensor.name,val);
}

bool AlterozoomSensorDataTranslator::checkConfig(const ARpcISensorStorage::DataExportConfig &cfg)
{
	return cfg.contains("email")&&!config["email"].isEmpty();
}

QByteArray AlterozoomSensorDataTranslator::type()const
{
	return mType;
}
