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

bool EngineCallbacks::commandCallbackWaitAnswer(const QUuid &devId,const QByteArray &cmd,
	const QByteArrayList &args,QByteArrayList &retVal)
{
	RealDevice *d=srv->devices()->devById(devId);
	if(!d)return false;
	auto c=d->execCommand(cmd,args);
	bool ok=c->wait();
	retVal=c->returnValue();
	return ok;
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
