#include "ARpcVirtualDevice.h"
#include "ARpcConfig.h"

ARpcVirtualDevice::ARpcVirtualDevice(const QUuid &id,const QString &name,const QList<ARpcSensor> &sensors,
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

bool ARpcVirtualDevice::writeMsg(const ARpcMessage &m)
{
	if(m.title==ARpcConfig::identifyMsg)
		writeMsgFromDevice({ARpcConfig::deviceInfoMsg,QStringList()<<devId.toString()<<devName});
	else if(m.title==ARpcConfig::funcCallMsg)
	{
		if(m.args.count()<1||m.args[0].length()==0)
			writeErr(QStringList()<<"No command");
		else if(m.args[0][0]=='#')
		{
			if(m.args[0]=="#sensors")
				writeOk(QStringList()<<sensorsXml);
			else if(m.args[0]=="#controls")
				writeOk(QStringList()<<controlsXml);
		}
		else
		{
			bool ok=false;
			QString cmd=m.args[0];
			QStringList args=m.args;
			args.removeAt(0);
			QStringList retVal;
			emit processDeviceCommand(cmd,m.args,ok,retVal);
			if(ok)
				writeOk(retVal);
			else if(retVal.isEmpty())
				writeErr(QStringList()<<"unknown error");
			else
				writeErr(retVal);
		}
	}
	else
		writeInfo(QStringList()<<"ERROR: unknown message");
	return true;
}

bool ARpcVirtualDevice::isConnected()
{
	return true;
}

void ARpcVirtualDevice::writeMsgFromDevice(const ARpcMessage &m)
{
	emit rawMessage(m);
}

void ARpcVirtualDevice::writeOk(const QStringList &args)
{
	writeMsgFromDevice({ARpcConfig::funcAnswerOkMsg,args});
}

void ARpcVirtualDevice::writeErr(const QStringList &args)
{
	writeMsgFromDevice({ARpcConfig::funcAnswerErrMsg,args});
}

void ARpcVirtualDevice::writeInfo(const QStringList &args)
{
	writeMsgFromDevice({ARpcConfig::infoMsg,args});
}

void ARpcVirtualDevice::writeMeasurement(const QString &name,const QStringList &values)
{
	writeMsgFromDevice({ARpcConfig::measurementMsg,QStringList()<<name<<values});
}

void ARpcVirtualDevice::writeSync()
{
	writeMsgFromDevice({ARpcConfig::funcSyncMsg});
}
