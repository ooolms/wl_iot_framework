/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "ARpcRealDevice.h"
#include "ARpcBase/ARpcSyncCall.h"
#include <QTimer>
#include <QEventLoop>

ARpcRealDevice::ARpcRealDevice(QObject *parent)
	:ARpcDevice(parent)
{
	identifyTimer.setInterval(2000);
	identifyTimer.setSingleShot(true);

	connect(this,&ARpcRealDevice::connected,this,&ARpcRealDevice::identify);
	connect(this,&ARpcRealDevice::disconnected,this,&ARpcRealDevice::onDisconnected);
}

bool ARpcRealDevice::identify()
{
	if(!isConnected())return false;
	identifyTimer.stop();
	devId=QUuid();
	devName.clear();
	QStringList retVal;
	QObject o;
	connect(&o,&QObject::destroyed,[this]()
	{
		if(!isIdentified())identifyTimer.start();
	});
	{//call identification
		QTimer t(this);
		t.setInterval(ARpcConfig::identifyWaitTime);
		t.setSingleShot(true);
		QEventLoop loop;
		bool ok=false;
		auto conn1=connect(this,&ARpcRealDevice::rawMessage,this,[&t,&loop,this,&ok,&retVal](const ARpcMessage &m)
		{
			if(m.title!="deviceinfo")return;
			if(m.args.count()<2)
			{
				ok=false;
				loop.quit();
			}
			ok=true;
			retVal=m.args;
			loop.quit();
		});
		connect(&t,&QTimer::timeout,&loop,&QEventLoop::quit);
		connect(this,&ARpcRealDevice::disconnected,&loop,&QEventLoop::quit);
		t.start();
		writeMsg("identify");
		loop.exec(QEventLoop::ExcludeUserInputEvents);
		disconnect(conn1);
		if(!ok)return false;
	}
	bool tmpHubDevice=(retVal[0]=="#hub");//TODO move "#hub" to ARpcConfig
	if(tmpHubDevice)
	{
		retVal.removeAt(0);
		if(retVal.count()<2)return false;
	}
	hubDevice=tmpHubDevice;
	if(retVal[1].isEmpty())return false;
	if(retVal[0].startsWith('{'))
		devId=QUuid(retVal[0]);
	else devId=QUuid::fromRfc4122(QByteArray::fromHex(retVal[0].toUtf8()));
	if(devId.isNull())return false;
	devName=retVal[1];
	emit identificationChanged();
	return true;
}

void ARpcRealDevice::resetIdentification()
{
	devId=QUuid();
	devName.clear();
	emit identificationChanged();
}

void ARpcRealDevice::onDisconnected()
{
	identifyTimer.stop();
}

bool ARpcRealDevice::isIdentified()
{
	return !devId.isNull();
}

QUuid ARpcRealDevice::id()
{
	return devId;
}

QString ARpcRealDevice::name()
{
	return devName;
}

bool ARpcRealDevice::getSensorsDescription(QList<ARpcSensor> &sensors)
{
	QStringList retVal;
	ARpcSyncCall call;
	if(!call.call(this,ARpcConfig::getSensorsCommand,retVal))return false;
	if(retVal.count()<1)return false;
	retVal[0]=retVal[0].trimmed();
	if(retVal[0].startsWith('{'))return ARpcSensor::parseJsonDescription(retVal[0],sensors);
	return ARpcSensor::parseXmlDescription(retVal[0],sensors);
}

bool ARpcRealDevice::getControlsDescription(ARpcControlsGroup &controls)
{
	QStringList retVal;
	ARpcSyncCall call;
	if(!call.call(this,ARpcConfig::getControlsCommand,retVal))return false;
	if(retVal.count()<1)return false;
	retVal[0]=retVal[0].trimmed();
	if(retVal[0].startsWith('{'))return ARpcControlsGroup::parseJsonDescription(retVal[0],controls);
	return ARpcControlsGroup::parseXmlDescription(retVal[0],controls);
}

bool ARpcRealDevice::getState(ARpcDeviceState &state)
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

bool ARpcRealDevice::isHubDevice()
{
	return hubDevice;
}
