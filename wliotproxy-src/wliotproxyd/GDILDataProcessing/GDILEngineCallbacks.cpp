#include "GDILEngineCallbacks.h"
#include "../ServerInstance.h"

using namespace WLIOT;

void GDILEngineCallbacks::commandCallback(const QUuid &devId,const QByteArray &cmd,const QByteArrayList &args)
{
	RealDevice *d=ServerInstance::inst().devices()->deviceById(devId);
	if(d)d->execCommand(cmd,args);
}

void GDILEngineCallbacks::debugCallback(const QString &msg)
{
	qDebug()<<"GDIL message: "<<msg;
}
