#include "ARpcVirtualDevice.h"
#include "ARpcConfig.h"

ARpcVirtualDevice::ARpcVirtualDevice(const QUuid &id,const QByteArray &name,const QList<ARpcSensorDef> &sensors,
	const ARpcControlsGroup &controls,QObject *parent)
	:ARpcRealDevice(parent)
{
	mId=id;
	mName=name;
	ARpcSensorDef::dumpToXml(sensorsXml,sensors);
	mSensors=sensors;
	ARpcControlsGroup::dumpToXml(controlsXml,controls);
	mControls=controls;
	resetIdentification(mId,mName);
}

void ARpcVirtualDevice::reconnect()
{
	emit identificationChanged(mId,mId);
}

bool ARpcVirtualDevice::writeMsg(const ARpcMessage &m)
{
	return QMetaObject::invokeMethod(this,"writeMsgQueued",Qt::QueuedConnection,Q_ARG(ARpcMessage,m));
}

void ARpcVirtualDevice::writeMsgQueued(ARpcMessage m)
{
	if(m.title==ARpcConfig::identifyMsg)
		writeMsgFromDevice({ARpcConfig::deviceInfoMsg,QByteArrayList()<<devId.toByteArray()<<devName});
	else if(m.title==ARpcConfig::devSyncMsg)
	{
		writeMsgFromDevice(ARpcConfig::devSyncrMsg);
		emit syncMsgNotify();
	}
	else if(m.title==ARpcConfig::funcCallMsg)
	{
		callIdStr.clear();
		if(m.args.count()<2||m.args[0].length()==0||m.args[1].length()==0)
		{
			writeErr(QByteArrayList()<<"No command");
			return;
		}
		callIdStr=m.args[0];
		if(m.args[1][0]=='#')
		{
			if(m.args[0]=="#sensors")
				writeOk(QByteArrayList()<<sensorsXml);
			else if(m.args[0]=="#controls")
				writeOk(QByteArrayList()<<controlsXml);
		}
		else
		{
			bool ok=false;
			QByteArray cmd=m.args[1];
			QByteArrayList args=m.args;
			args.removeAt(0);
			args.removeAt(1);
			QByteArrayList retVal;
			emit processDeviceCommand(cmd,m.args,ok,retVal);
			if(ok)
				writeOk(retVal);
			else
				writeErr(retVal);
		}
	}
	else
		writeInfo(QByteArrayList()<<"ERROR: unknown message");
}

bool ARpcVirtualDevice::isConnected()
{
	return true;
}

void ARpcVirtualDevice::setSensors(const QList<ARpcSensorDef> &s)
{
	mSensors=s;
	ARpcSensorDef::dumpToXml(sensorsXml,s);
	emit identificationChanged(mId,mId);
}

void ARpcVirtualDevice::setControls(const ARpcControlsGroup &c)
{
	mControls=c;
	ARpcControlsGroup::dumpToXml(controlsXml,c);
	emit identificationChanged(mId,mId);
}

void ARpcVirtualDevice::setSensors(const QByteArray &s)
{
	QList<ARpcSensorDef> sList;
	if(s.startsWith("{"))
	{
		if(!ARpcSensorDef::parseJsonDescription(s,sList))return;
	}
	else if(!ARpcSensorDef::parseXmlDescription(s,sList))return;
	mSensors=sList;
	ARpcSensorDef::dumpToXml(sensorsXml,sList);
	emit identificationChanged(mId,mId);
}

void ARpcVirtualDevice::setControls(const QByteArray &c)
{
	ARpcControlsGroup cGrp;
	if(c.startsWith("{"))
	{
		if(!ARpcControlsGroup::parseJsonDescription(c,cGrp))return;
	}
	else if(!ARpcControlsGroup::parseXmlDescription(c,cGrp))return;
	mControls=cGrp;
	ARpcControlsGroup::dumpToXml(controlsXml,cGrp);
	emit identificationChanged(mId,mId);
}

void ARpcVirtualDevice::writeMsgFromDevice(const ARpcMessage &m)
{
	emit rawMessage(m);
}

void ARpcVirtualDevice::writeOk(const QByteArrayList &args)
{
	writeMsgFromDevice({ARpcConfig::funcAnswerOkMsg,QByteArrayList()<<callIdStr<<args});
}

void ARpcVirtualDevice::writeErr(const QByteArrayList &args)
{
	writeMsgFromDevice({ARpcConfig::funcAnswerErrMsg,QByteArrayList()<<callIdStr<<args});
}

void ARpcVirtualDevice::writeInfo(const QByteArrayList &args)
{
	writeMsgFromDevice({ARpcConfig::infoMsg,args});
}

void ARpcVirtualDevice::writeMeasurement(const QByteArray &name,const QByteArrayList &values)
{
	writeMsgFromDevice({ARpcConfig::measurementMsg,QByteArrayList()<<name<<values});
}

void ARpcVirtualDevice::writeSync()
{
	writeMsgFromDevice({ARpcConfig::funcSynccMsg});
}
