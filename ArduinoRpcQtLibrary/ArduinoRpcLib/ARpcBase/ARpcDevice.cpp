#include "ARpcDevice.h"
#include "ARpcBase/ARpcSyncCall.h"
#include <QTimer>
#include <QEventLoop>

ARpcDevice::ARpcDevice(QObject *parent)
	:QObject(parent)
	,streamParser(&msgParser)
{
	identifyTimer.setInterval(2000);
	identifyTimer.setSingleShot(true);

	connect(&streamParser,&ARpcStreamParser::processMessage,this,&ARpcDevice::rawMessage);
	connect(this,&ARpcDevice::connected,this,&ARpcDevice::identify);
	connect(this,&ARpcDevice::disconnected,this,&ARpcDevice::onDisconnected);
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
	identifyTimer.stop();
	devId=QUuid();
	devName.clear();
	QStringList retVal;
	QObject o;
	connect(&o,&QObject::destroyed,[this]()
	{
		if(!isIdentified())identifyTimer.start();
	});
	if(!internallFunctionCall(ARpcMessage(ARpcConfig::identifyMsg),
		ARpcMessage(ARpcConfig::deviceInfoMsg),ARpcConfig::identifyWaitTime,retVal))return false;
	if(retVal.count()<2)return false;
	if(retVal[1].isEmpty())return false;
	if(retVal[0].startsWith('{'))
		devId=QUuid(retVal[0]);
	else devId=QUuid::fromRfc4122(QByteArray::fromHex(retVal[0].toUtf8()));
	if(devId.isNull())return false;
	devName=retVal[1];
	emit identificationChanged();
	return true;
}

void ARpcDevice::resetIdentification()
{
	devId=QUuid();
	devName.clear();
	emit identificationChanged();
}

void ARpcDevice::onDisconnected()
{
	identifyTimer.stop();
}

bool ARpcDevice::internallFunctionCall(const ARpcMessage &msg,const ARpcMessage &estimatedReturnMsg,
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
		[&t,&loop,this,&ok,&retVal,&estimatedReturnMsg](const ARpcMessage &m)
	{
		if(m.title!=estimatedReturnMsg.title)return;
		if(m.args.count()<estimatedReturnMsg.args.count())return;
		for(int i=0;i<estimatedReturnMsg.args.count();++i)
			if(m.args[i]!=estimatedReturnMsg.args[i])return;
		ok=true;
		retVal=m.args;
		for(int i=0;i<estimatedReturnMsg.args.count();++i)
			retVal.removeAt(0);
		loop.quit();
	});
	connect(&t,&QTimer::timeout,&loop,&QEventLoop::quit);
	connect(this,&ARpcDevice::disconnected,&loop,&QEventLoop::quit);
	t.start();
	writeMsg(msg);
	loop.exec(QEventLoop::ExcludeUserInputEvents);
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
	return devName;
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

bool ARpcDevice::getControlsDescription(ARpcControlsGroup &controls)
{
	QStringList retVal;
	ARpcSyncCall call;
	if(!call.call(this,ARpcConfig::getControlsCommand,retVal))return false;
	if(retVal.count()<1)return false;
	retVal[0]=retVal[0].trimmed();
	if(retVal[0].startsWith('{'))return ARpcControlsGroup::parseJsonDescription(retVal[0],controls);
	return ARpcControlsGroup::parseXmlDescription(retVal[0],controls);
}

bool ARpcDevice::getState(ARpcDeviceState &state)
{
	QStringList retVal;
	ARpcSyncCall call;
	if(!call.call(this,ARpcConfig::getStateCommand,retVal))return false;
	if(retVal.count()%3!=0)return false;
	state.additionalAttributes.clear();
	state.commandParams.clear();
	for(int i=0;i<retVal.count()/3;++i)
	{
		QString command=retVal[3*i];
		QString nameOrIndex=retVal[3*i+1];
		QString value=retVal[3*i+2];
		if(command.isEmpty())return false;
		else if(command=="#")
		{
			if(nameOrIndex.isEmpty())return false;
			state.additionalAttributes[nameOrIndex]=value;
		}
		else
		{
			bool ok=false;
			int index=nameOrIndex.toInt(&ok);
			if(!ok||index<=0)return false;
			state.commandParams[command][index]=value;
		}
	}
	return true;
}
