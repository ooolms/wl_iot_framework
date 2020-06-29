#include "VDILEngineCallbacks.h"
#include "../ServerInstance.h"

using namespace WLIOT;

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
