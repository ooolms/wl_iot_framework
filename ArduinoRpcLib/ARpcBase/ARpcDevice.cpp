#include "ARpcDevice.h"
#include "ARpcSimpleAPI/ARpcSyncCall.h"
#include "ARpcSimpleAPI/ARpcSyncUnsafeCall.h"

ARpcDevice::ARpcDevice(const ARpcConfig &cfg,QObject *parent)
	:QObject(parent)
	,config(cfg)
	,msgParser(cfg)
	,streamParser(cfg,&msgParser)
{
	connect(&streamParser,SIGNAL(processMessage(ARpcMessage)),this,SLOT(processMessage(ARpcMessage)));
}

bool ARpcDevice::writeMsg(const QString &msg)
{
	return writeMsg(ARpcMessage(msg));
}

bool ARpcDevice::writeMsg(const QString &msg,const QStringList &args)
{
	return writeMsg(ARpcMessage(msg,args));
}

void ARpcDevice::processMessage(const ARpcMessage &m)
{
	emit rawMessage(m);
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
