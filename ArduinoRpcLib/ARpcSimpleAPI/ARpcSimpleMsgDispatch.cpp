#include "ARpcSimpleMsgDispatch.h"

ARpcSimpleMsgDispatch::ARpcSimpleMsgDispatch(const ARpcConfig &cfg,ARpcDevice *dev,QObject *parent)
	:QObject(parent)
	,config(cfg)
{
	device=dev;
	connect(device,&ARpcDevice::rawMessage,this,&ARpcSimpleMsgDispatch::onRawMsg);
}

void ARpcSimpleMsgDispatch::onRawMsg(const ARpcMessage &m)
{
	if(m.title==config.infoMsgTitle&&m.args.count()>=1)
		emit infoMsg(m.args[0]);
	else if(m.title==config.measurementMsgTitle&&m.args.count()>=2)
		emit measurementMsg(m.args[0],m.args[1]);
}
