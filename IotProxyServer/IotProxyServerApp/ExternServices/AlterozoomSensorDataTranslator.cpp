#include "AlterozoomSensorDataTranslator.h"
#include "AlterozoomAuthentificationStorage.h"
#include <QDebug>

const QByteArray AlterozoomSensorDataTranslator::mType="alterozoom";

AlterozoomSensorDataTranslator::AlterozoomSensorDataTranslator(
	const QUuid &devId,const ARpcSensorDef &sens,const ARpcISensorStorage::DataExportConfig &cfg,QObject *parent)
	:ISensorDataTranslator(devId,sens,cfg,parent)
{
	host="alterozoom.com";
	if(config.contains("host")&&!config["host"].isEmpty())
		host=config["host"];
	email=config["email"];
	AlterozoomAuthentificationStorage::AuthKey k={host,email};
	if(AlterozoomAuthentificationStorage::getAuthMap().contains(k))
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
