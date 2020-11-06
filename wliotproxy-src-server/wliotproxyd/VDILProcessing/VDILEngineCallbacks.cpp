#include "VDILEngineCallbacks.h"
#include "VDILEngine.h"
#include "../ServerInstance.h"
#include "../MainServerConfig.h"

using namespace WLIOT;

VDILEngineCallbacks::VDILEngineCallbacks(IdType uid,VDILEngine *e)
{
	mEngine=e;
	mUid=uid;
}

void VDILEngineCallbacks::setProgramName(const QByteArray &name)
{
	mProgramName=name;
}

void VDILEngineCallbacks::commandCallback(const QUuid &devId,const QByteArray &cmd,const QByteArrayList &args)
{
	if(!MainServerConfig::accessManager.userCanAccessDevice(devId,mUid,DevicePolicyActionFlag::EXECUTE_COMMANDS))
		return;
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

bool VDILEngineCallbacks::commandCallbackWaitAnswer(const QUuid &devId, const QByteArray &cmd, const QByteArrayList &args, QByteArrayList &retVal)
{

}
