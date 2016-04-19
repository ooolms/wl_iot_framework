#include "ARpcDevice.h"
#include "ARpcSyncCall.h"
#include "ARpcSyncUnsafeCall.h"

ARpcDevice::ARpcDevice(const ARpcConfig &cfg,QObject *parent)
	:QObject(parent)
{
	config=cfg;
}

void ARpcDevice::processMessage(const ARpcMessage &m)
{
	emit rawMessage(m);
	if(m.title==config.infoMsgTitle)emit infoMessage(m);
}

bool ARpcDevice::callSync(const ARpcMessage &m,QStringList &retVal)
{
	if(!isConnected())return false;
	ARpcSyncCall call(config);
	return call.call(m,this,retVal);
}

bool ARpcDevice::callSyncUnsafe(const ARpcMessage &m,QStringList &retVal)
{
	if(!isConnected())return false;
	ARpcSyncUnsafeCall call(config);
	return call.call(m,this,retVal);
}
