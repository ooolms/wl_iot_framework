#include "ARpcDevice.h"

ARpcDevice::ARpcDevice(const ARpcConfig &cfg,QObject *parent)
	:QObject(parent)
	,config(cfg)
	,msgParser(cfg)
	,streamParser(cfg,&msgParser)
{
	connect(&streamParser,&ARpcStreamParser::processMessage,this,&ARpcDevice::rawMessage);
}

bool ARpcDevice::writeMsg(const QString &msg)
{
	return writeMsg(ARpcMessage(msg));
}

bool ARpcDevice::writeMsg(const QString &msg,const QStringList &args)
{
	return writeMsg(ARpcMessage(msg,args));
}

const ARpcConfig ARpcDevice::cfg()const
{
	return config;
}
