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
#include "ARpcHubDevice.h"
#include <QTimer>
#include <QEventLoop>

ARpcRealDevice::ARpcRealDevice(QObject *parent)
	:ARpcDevice(parent)
{
	hubDevice=false;
	identifyTimer.setInterval(2000);
	identifyTimer.setSingleShot(true);
	syncTimer.setInterval(6000);
	mControlsLoaded=mSensorsLoaded=false;
	mWasSyncr=false;
	connect(this,&ARpcRealDevice::rawMessage,this,&ARpcRealDevice::onRawMessage);
	connect(this,&ARpcRealDevice::connected,this,&ARpcRealDevice::identify);
	connect(this,&ARpcRealDevice::disconnected,this,&ARpcRealDevice::onDisconnected);
	connect(&syncTimer,&QTimer::timeout,this,&ARpcRealDevice::onSyncTimer,Qt::QueuedConnection);
}

bool ARpcRealDevice::identify()
{
	if(!isConnected())return false;
	identifyTimer.stop();
	devId=QUuid();
	devName.clear();
	QByteArrayList retVal;
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
			if(m.title!=ARpcConfig::deviceInfoMsg)return;
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
		writeMsg(ARpcConfig::identifyMsg);
		loop.exec(QEventLoop::ExcludeUserInputEvents);
		disconnect(conn1);
		if(!ok)return false;
	}
	bool tmpHubDevice=(retVal[0]==ARpcConfig::hubMsg);
	if(tmpHubDevice)
	{
		retVal.removeAt(0);
		if(retVal.count()<2)return false;
	}
	hubDevice=tmpHubDevice;
	if(retVal[1].isEmpty())return false;
	QUuid newId;
	QByteArray newName;
	if(retVal[0].startsWith('{'))
		newId=QUuid(retVal[0]);
	else newId=QUuid::fromRfc4122(QByteArray::fromHex(retVal[0]));
	if(newId.isNull())return false;
	newName=retVal[1];
	resetIdentification(newId,newName);
	mWasSyncr=true;
	syncTimer.start();
	return true;
}

void ARpcRealDevice::resetIdentification(QUuid newId,QByteArray newName)
{
	mSensorsLoaded=mControlsLoaded=false;
	std::swap(devId,newId);
	std::swap(devName,newName);
	emit identificationChanged(newId,devId);
}

void ARpcRealDevice::syncFailed()
{
}

void ARpcRealDevice::onDisconnected()
{
	mSensorsLoaded=mControlsLoaded=false;
	mWasSyncr=false;
	identifyTimer.stop();
	syncTimer.stop();
	streamParser.reset();
}

void ARpcRealDevice::onRawMessage(const ARpcMessage &m)
{
	if(hubDevice&&m.title==ARpcConfig::hubMsg)
		onHubMsg(m);
	else if(m.title==ARpcConfig::devSyncrMsg)
		mWasSyncr=true;
}

void ARpcRealDevice::onSyncTimer()
{
	if(mWasSyncr)
	{
		mWasSyncr=false;
		writeMsg(ARpcConfig::devSyncMsg);
	}
	else
	{
		mSensorsLoaded=mControlsLoaded=false;
		mWasSyncr=false;
		identifyTimer.start();
		syncTimer.stop();
		streamParser.reset();
		resetIdentification();
		syncFailed();
		emit disconnected();
	}
}

void ARpcRealDevice::onHubMsg(const ARpcMessage &m)
{
	if(m.args.count()<2)return;
	QUuid id;
	if(m.args[0].startsWith('{'))
		id=QUuid(m.args[0]);
	else id=QUuid::fromRfc4122(QByteArray::fromHex(m.args[0]));
	if(id.isNull()||m.args[1].isEmpty())return;
	if(m.args[1]==ARpcConfig::hubDeviceIdentifiedMsg)
	{
		if(m.args.count()<3)return;
		onHubDeviceIdentified(id,m.args[2]);
	}
	else if(m.args[1]==ARpcConfig::hubDeviceLostMsg)
	{
		if(!hubDevicesMap.contains(id))return;
		hubDevicesMap[id]->setSelfConnected(false);
		emit childDeviceLost(id);
	}
	else if(m.args[1]==ARpcConfig::deviceInfoMsg)
	{
		if(m.args.count()<4)return;
		onHubDeviceIdentified(id,m.args[3]);
	}
	else
	{
		if(!hubDevicesMap.contains(id))return;
		ARpcMessage m2=m;
		m2.title=m.args[1];
		m2.args.removeFirst();
		m2.args.removeFirst();
		hubDevicesMap[id]->onRawMessage(m2);
	}
}

void ARpcRealDevice::onHubDeviceIdentified(const QUuid &id,const QByteArray &name)
{
	if(!hubDevicesMap.contains(id))
	{
		hubDevicesMap[id]=0;
		ARpcHubDevice *dev=new ARpcHubDevice(id,name,this);
		hubDevicesMap[id]=dev;
		dev->setSelfConnected(true);
		emit childDeviceIdentified(id);
	}
	else
	{
		ARpcHubDevice *dev=hubDevicesMap[id];
		if(dev->isConnected()&&dev->sensorsLoaded())
			return;
		dev->setSelfConnected(true);
		emit childDeviceIdentified(id);
	}
}

bool ARpcRealDevice::identifyHub()
{
	if(!isConnected()||devId.isNull())return false;
	ARpcSyncCall call(this);
	return call.call(ARpcConfig::identifyHubMsg,false);
//	for(auto &i:hubDevicesMap)
//		delete i;
//	hubDevicesMap.clear();
//	QByteArrayList retVal;
//	{//call identification
//		QTimer t(this);
//		t.setInterval(ARpcConfig::identifyWaitTime*2);
//		t.setSingleShot(true);
//		QEventLoop loop;
//		auto conn1=connect(this,&ARpcRealDevice::rawMessage,this,[&t,&loop,this,&retVal](const ARpcMessage &m)
//		{
//			if(m.title!=ARpcConfig::hubMsg)return;
//			onHubMsg(m);
//		});
//		connect(&t,&QTimer::timeout,&loop,&QEventLoop::quit);
//		connect(this,&ARpcRealDevice::disconnected,&loop,&QEventLoop::quit);
//		t.start();
//		writeMsg(ARpcConfig::identifyHubMsg);
//		loop.exec(QEventLoop::ExcludeUserInputEvents);
//		disconnect(conn1);
//	}
	//	return true;
}

bool ARpcRealDevice::controlsLoaded()
{
	return mControlsLoaded;
}

bool ARpcRealDevice::sensorsLoaded()
{
	return mSensorsLoaded;
}

bool ARpcRealDevice::isIdentified()
{
	return !devId.isNull();
}

QUuid ARpcRealDevice::id()
{
	return devId;
}

QByteArray ARpcRealDevice::name()
{
	return devName;
}

bool ARpcRealDevice::getSensorsDescription(QList<ARpcSensor> &sensors)
{
	if(mSensorsLoaded)
	{
		sensors=mSensors;
		return true;
	}
	ARpcSyncCall call(this);
	if(!call.call(ARpcConfig::getSensorsCommand))return false;
	QByteArrayList retVal=call.returnValue();
	if(retVal.count()<1)return false;
	retVal[0]=retVal[0].trimmed();
	if(retVal[0].startsWith('{'))
	{
		if(!ARpcSensor::parseJsonDescription(retVal[0],mSensors))return false;
	}
	else if(!ARpcSensor::parseXmlDescription(retVal[0],mSensors))return false;
	mSensorsLoaded=true;
	sensors=mSensors;
	return true;
}

bool ARpcRealDevice::getControlsDescription(ARpcControlsGroup &controls)
{
	if(mControlsLoaded)
	{
		controls=mControls;
		return true;
	}
	ARpcSyncCall call(this);
	if(!call.call(ARpcConfig::getControlsCommand))return false;
	QByteArrayList retVal=call.returnValue();
	if(retVal.count()<1)return false;
	retVal[0]=retVal[0].trimmed();
	if(retVal[0].startsWith('{'))
	{
		if(!ARpcControlsGroup::parseJsonDescription(retVal[0],mControls))return false;
	}
	else if(!ARpcControlsGroup::parseXmlDescription(retVal[0],mControls))return false;
	mControlsLoaded=true;
	controls=mControls;
	return true;
}

bool ARpcRealDevice::getState(ARpcDeviceState &state)
{
	ARpcSyncCall call(this);
	if(!call.call(ARpcConfig::getStateCommand))return false;
	QByteArrayList retVal=call.returnValue();
	if(retVal.count()%3!=0)return false;
	state.additionalAttributes.clear();
	state.commandParams.clear();
	for(int i=0;i<retVal.count()/3;++i)
	{
		QByteArray command=retVal[3*i];
		QByteArray nameOrIndex=retVal[3*i+1];
		QByteArray value=retVal[3*i+2];
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

QList<QUuid> ARpcRealDevice::childDevices()
{
	return hubDevicesMap.keys();
}

ARpcRealDevice* ARpcRealDevice::childDevice(const QUuid &id)
{
	return hubDevicesMap.value(id,0);
}
