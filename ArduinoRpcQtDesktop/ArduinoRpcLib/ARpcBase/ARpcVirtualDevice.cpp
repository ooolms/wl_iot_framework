#include "ARpcVirtualDevice.h"
#include "ARpcConfig.h"

ARpcVirtualDevice::ARpcVirtualDevice(const QUuid &id,const QByteArray &name,const QList<ARpcSensor> &sensors,
	const ARpcControlsGroup &controls,QObject *parent)
	:ARpcRealDevice(parent)
{
	mId=id;
	mName=name;
	ARpcSensor::dumpToXml(sensorsXml,sensors);
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
		writeMsgFromDevice(ARpcConfig::devSyncrMsg);
	else if(m.title==ARpcConfig::funcCallMsg)
	{
		if(m.args.count()<1||m.args[0].length()==0)
			writeErr(QByteArrayList()<<"No command");
		else if(m.args[0][0]=='#')
		{
			if(m.args[0]=="#sensors")
				writeOk(QByteArrayList()<<sensorsXml);
			else if(m.args[0]=="#controls")
				writeOk(QByteArrayList()<<controlsXml);
		}
		else
		{
			bool ok=false;
			QByteArray cmd=m.args[0];
			QByteArrayList args=m.args;
			args.removeAt(0);
			QByteArrayList retVal;
			emit processDeviceCommand(cmd,m.args,ok,retVal);
			if(ok)
				writeOk(retVal);
			else if(retVal.isEmpty())
				writeErr(QByteArrayList()<<"unknown error");
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

void ARpcVirtualDevice::setSensors(const QList<ARpcSensor> &s)
{
	mSensors=s;
	ARpcSensor::dumpToXml(sensorsXml,s);
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
	QList<ARpcSensor> sList;
	if(s.startsWith("{"))
	{
		if(!ARpcSensor::parseJsonDescription(s,sList))return;
	}
	else if(!ARpcSensor::parseXmlDescription(s,sList))return;
	mSensors=sList;
	ARpcSensor::dumpToXml(sensorsXml,sList);
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
	writeMsgFromDevice({ARpcConfig::funcAnswerOkMsg,args});
}

void ARpcVirtualDevice::writeErr(const QByteArrayList &args)
{
	writeMsgFromDevice({ARpcConfig::funcAnswerErrMsg,args});
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
