#include "VDILEngineCallbacks.h"
#include "VDILEngine.h"
#include "../ServerInstance.h"

using namespace WLIOT;

VDILEngineCallbacks::VDILEngineCallbacks(VDILEngine *e)
{
	mEngine=e;
}

void VDILEngineCallbacks::setProgramName(const QByteArray &name)
{
	mProgramName=name;
}

void VDILEngineCallbacks::commandCallback(const QUuid &devId,const QByteArray &cmd,const QByteArrayList &args)
{
	RealDevice *d=ServerInstance::inst().devices()->deviceById(devId);
	if(d)d->execCommand(cmd,args);
}

void VDILEngineCallbacks::debugCallback(const QString &msg)
{
	qDebug()<<"VDIL debug: "<<mProgramName<<": "<<msg;
}

void VDILEngineCallbacks::sendVDevMeasurementB(const QByteArray &sensorName,const QByteArray &data)
{
	mEngine->sendVDevMeasurementB(sensorName,data);
}
