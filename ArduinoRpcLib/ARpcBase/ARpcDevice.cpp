#include "ARpcDevice.h"
#include "ARpcBase/ARpcSyncCall.h"
#include <QTimer>
#include <QEventLoop>

ARpcDevice::ARpcDevice(QObject *parent)
	:QObject(parent)
	,streamParser(&msgParser)
{
	connect(&streamParser,&ARpcStreamParser::processMessage,this,&ARpcDevice::rawMessage);
	connect(this,&ARpcDevice::connected,this,&ARpcDevice::identify);
}

bool ARpcDevice::writeMsg(const QString &msg)
{
	return writeMsg(ARpcMessage(msg));
}

bool ARpcDevice::writeMsg(const QString &msg,const QStringList &args)
{
	return writeMsg(ARpcMessage(msg,args));
}

bool ARpcDevice::identify()
{
	devId=QUuid();
	deviceName.clear();
	QStringList retVal;
	if(!internallFunctionCall(ARpcMessage(ARpcConfig::identifyMsg),
		ARpcConfig::deviceInfoMsg,ARpcConfig::identifyWaitTime,retVal))return false;
	if(retVal.count()<2)return false;
	if(retVal[1].isEmpty())return false;
	devId=QUuid(retVal[0]);
	if(devId.isNull())return false;
	deviceName=retVal[1];
	return true;
}

bool ARpcDevice::internallFunctionCall(const ARpcMessage &msg,const QString &estimatedReturnMsg,
	int timeout,QStringList &retVal)
{
	if(!isConnected())return false;
	QTimer t(this);
	t.setInterval(timeout);
	t.setSingleShot(true);
	QEventLoop loop;
	bool ok=false;
	retVal.clear();
	auto conn1=connect(this,&ARpcDevice::rawMessage,this,
		[&t,&loop,this,&ok,&retVal,&estimatedReturnMsg](const ARpcMessage &m){
			if(m.title==estimatedReturnMsg)
			{
				ok=true;
				retVal=m.args;
				loop.quit();
			}
	});
	connect(&t,&QTimer::timeout,&loop,&QEventLoop::quit);
	connect(this,&ARpcDevice::disconnected,&loop,&QEventLoop::quit);
	t.start();
	writeMsg(msg);
	loop.exec();
	disconnect(conn1);
	return ok;
}

bool ARpcDevice::isIdentified()
{
	return !devId.isNull();
}

QUuid ARpcDevice::id()
{
	return devId;
}

QString ARpcDevice::name()
{
	return deviceName;
}

bool ARpcDevice::getSensorsDescription(QList<ARpcSensor> &sensors)
{
	QStringList retVal;
	ARpcSyncCall call;
	if(!call.call(this,ARpcConfig::getSensorsCommand,retVal))return false;
	if(retVal.count()<1)return false;
	retVal[0]=retVal[0].trimmed();
	if(retVal[0].startsWith('{'))return ARpcSensor::parseJsonDescription(retVal[0],sensors);
	return ARpcSensor::parseXmlDescription(retVal[0],sensors);
}

bool ARpcDevice::getControlsDescription(QList<ARpcControlsGroup> &controls)
{
	QStringList retVal;
	ARpcSyncCall call;
	if(!call.call(this,ARpcConfig::getSensorsCommand,retVal))return false;
	if(retVal.count()<1)return false;
	retVal[0]=retVal[0].trimmed();
	if(retVal[0].startsWith('{'))return ARpcControlsGroup::parseJsonDescription(retVal[0],controls);
	return ARpcSensor::parseXmlDescription(retVal[0],controls);
}
