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
	else if(m.title==ARpcConfig::stateChangedMsg&&m.args.count()%3==0)
	{
		for(int i=0;i<m.args.count()/3;++i)
		{
			QString command=m.args[3*i];
			QString nameOrIndex=m.args[3*i+1];
			QString value=m.args[3*i+2];
			if(command.isEmpty())return;
			else if(command=="#")
			{
				if(nameOrIndex.isEmpty())return;
				emit additionalStateChanged(nameOrIndex,value);
			}
			else
			{
				bool ok=false;
				int index=nameOrIndex.toInt(&ok);
				if(!ok||index<=0)return;
				emit commandStateChanged(command,index,value);
			}
		}
	}
}
