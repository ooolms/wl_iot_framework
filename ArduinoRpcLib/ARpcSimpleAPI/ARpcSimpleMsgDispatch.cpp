#include "ARpcSimpleMsgDispatch.h"

ARpcSimpleMsgDispatch::ARpcSimpleMsgDispatch(ARpcDevice *dev,QObject *parent)
	:QObject(parent)
{
	device=dev;
	connect(device,&ARpcDevice::rawMessage,this,&ARpcSimpleMsgDispatch::onRawMsg);
}

void ARpcSimpleMsgDispatch::onRawMsg(const ARpcMessage &m)
{
	if(m.title==ARpcConfig::infoMsg&&m.args.count()>=1)
		emit infoMsg(m.args[0]);
	else if(m.title==ARpcConfig::measurementMsg&&m.args.count()>=2)
		emit measurementMsg(m.args[0],m.args[1]);
}
