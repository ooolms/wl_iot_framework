#include "EngineCallbacks.h"

using namespace WLIOT;
using namespace WLIOTClient;

EngineCallbacks::EngineCallbacks(WLIOTClient::ServerInstance *s)
{
	srv=s;
}

void EngineCallbacks::setProgramName(const QByteArray &name)
{
	mProgramName=name;
}

void EngineCallbacks::commandCallback(const QUuid &devId,const QByteArray &cmd,const QByteArrayList &args)
{
	RealDevice *d=srv->devices()->devById(devId);
	if(d)d->execCommand(cmd,args);
}

void EngineCallbacks::debugCallback(const QString &msg)
{
	qDebug()<<"VDIL debug: "<<mProgramName<<": "<<msg;
}

void EngineCallbacks::sendVDevMeasurementB(const QByteArray &sensorName,const QByteArray &data)
{
	if(vDev)
		vDev->sendVDevMeasurementB(sensorName,data);
}
