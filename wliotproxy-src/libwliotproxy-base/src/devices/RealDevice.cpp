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

#include "wliot/devices/RealDevice.h"
#include "wliot/devices/CommandCall.h"
#include "wliot/devices/HubDevice.h"
#include <QTimer>
#include <QEventLoop>
#include <QDebug>

RealDevice::RealDevice(QObject *parent)
	:QObject(parent)
{
	hubDevice=false;
	tryIdentifyTimer.setInterval(2000);
	tryIdentifyTimer.setSingleShot(false);
	syncTimer.setInterval(WLIOTProtocolDefs::syncWaitTime);
	syncTimer.setSingleShot(false);
	mControlsLoaded=mSensorsLoaded=mStateLoaded=false;
	mWasSyncr=false;
	mConnected=false;
	connect(&syncTimer,&QTimer::timeout,this,&RealDevice::onSyncTimer,Qt::QueuedConnection);
	connect(&tryIdentifyTimer,&QTimer::timeout,this,&RealDevice::onIdentifyTimer,Qt::QueuedConnection);
}

RealDevice::~RealDevice()
{
	;
}

RealDevice::IdentifyResult RealDevice::identify()
{
	if(!isConnected())
		return FAILED;
	tryIdentifyTimer.stop();
	devId=QUuid();
	devName.clear();
	QByteArrayList retVal;
	QObject o;
	connect(&o,&QObject::destroyed,[this]()
	{
		if(!isIdentified())
			tryIdentifyTimer.start();
	});
	{//call identification
		QTimer t(this);
		t.setInterval(WLIOTProtocolDefs::identifyWaitTime);
		t.setSingleShot(true);
		QEventLoop loop;
		bool ok=false;
		auto conn1=connect(this,&RealDevice::newMessageFromDevice,this,[&t,&loop,this,&ok,&retVal](const Message &m)
		{
			if(m.title!=WLIOTProtocolDefs::deviceInfoMsg)return;
			if(m.args.count()<2)
			{
				ok=false;
				loop.quit();
			}
			ok=true;
			retVal=m.args;
			loop.quit();
		});
		auto conn2=connect(this,&RealDevice::deviceWasReset,this,[this]()
		{
			writeMsgToDevice(WLIOTProtocolDefs::identifyMsg);
		});
		connect(&t,&QTimer::timeout,&loop,&QEventLoop::quit);
		connect(this,&RealDevice::disconnected,&loop,&QEventLoop::quit);
		t.start();
		writeMsgToDevice(WLIOTProtocolDefs::identifyMsg);
		loop.exec(QEventLoop::ExcludeUserInputEvents);
		disconnect(conn1);
		disconnect(conn2);
		if(!ok)return FAILED;
	}
	bool tmpHubDeviceFlag=(retVal[0]==WLIOTProtocolDefs::hubMsg);
	if(tmpHubDeviceFlag)
	{
		retVal.removeAt(0);
		if(retVal.count()<2)
			return FAILED;
	}
	hubDevice=tmpHubDeviceFlag;
	if(retVal[1].isEmpty())
		return OK_NULL_ID_OR_NAME;
	QUuid newId;
	QByteArray newName;
	if(retVal[0].startsWith('{'))
		newId=QUuid(retVal[0]);
	else newId=QUuid::fromRfc4122(QByteArray::fromHex(retVal[0]));
	if(newId.isNull())
		return OK_NULL_ID_OR_NAME;
	newName=retVal[1];
	QUuid newTypeId;
	if(retVal.count()>2)
	{
		if(retVal[2].startsWith('{'))
			newTypeId=QUuid(retVal[2]);
		else newTypeId=QUuid::fromRfc4122(QByteArray::fromHex(retVal[2]));

	}
	resetIdentification(newId,newName,newTypeId);
	return OK;
}

void RealDevice::resetIdentification(QUuid newId,QByteArray newName,QUuid newTypeId)
{
	mSensorsLoaded=mControlsLoaded=mStateLoaded=false;
	std::swap(devId,newId);
	std::swap(devName,newName);
	std::swap(devTypeId,newTypeId);
	emit identificationChanged(newId,devId);
	if(!devId.isNull()&&!syncTimer.isActive())
	{
		mWasSyncr=true;
		syncTimer.start();
	}
}

void RealDevice::syncFailed()
{
}

void RealDevice::onConnected()
{
	//TODO ???
	mConnected=true;
	emit connected();
	onDeviceReset();
	if(devId.isNull())
		identify();
	else
	{
		mWasSyncr=true;
		syncTimer.start();
	}
}

void RealDevice::onDisconnected()
{
	mConnected=false;
	mSensorsLoaded=mControlsLoaded=mStateLoaded=false;
	mWasSyncr=false;
	tryIdentifyTimer.stop();
	syncTimer.stop();
	emit disconnected();
}

void RealDevice::onNewMessage(const Message &m)
{
	if(hubDevice&&m.title==WLIOTProtocolDefs::hubMsg)
		onHubMsg(m);
	else if(m.title==WLIOTProtocolDefs::devSyncrMsg)
		mWasSyncr=true;
	else if(m.title==WLIOTProtocolDefs::stateChangedMsg)
	{
		if((m.args.count()%3)!=0)return;
		for(int i=0;i<m.args.count()/3;++i)
		{
			QByteArray command=m.args[3*i];
			QByteArray nameOrIndex=m.args[3*i+1];
			QByteArray value=m.args[3*i+2];
			if(command.isEmpty())continue;
			else if(command=="#")
			{
				if(nameOrIndex.isEmpty())continue;
				mState.additionalAttributes[nameOrIndex]=value;
			}
			else
			{
				bool ok=false;
				int index=nameOrIndex.toInt(&ok);
				if(!ok||index<=0)continue;
				mState.commandParams[command][index]=value;
			}
		}
		emit stateChanged(m.args);
	}
	else emit newMessageFromDevice(m);
}

void RealDevice::onDeviceReset()
{
	mSensorsLoaded=mControlsLoaded=mStateLoaded=false;
	emit deviceWasReset();
}

void RealDevice::onSyncTimer()
{
	if(mWasSyncr)
	{
		mWasSyncr=false;
		writeMsgToDevice(WLIOTProtocolDefs::devSyncMsg);
	}
	else
	{
		syncFailed();
		onDisconnected();
	}
}

void RealDevice::onChildDeviceSyncFailed()
{
	HubDevice *d=(HubDevice*)sender();
	if(!hubDevicesMap.contains(d->id())||hubDevicesMap[d->id()]!=d)return;
	d->setSelfConnected(false);
	emit childDeviceLost(d->id());
}

void RealDevice::onIdentifyTimer()
{
	identify();
}

void RealDevice::onHubMsg(const Message &m)
{
	if(m.args.count()<2)return;
	QUuid id;
	if(m.args[0].startsWith('{'))
		id=QUuid(m.args[0]);
	else id=QUuid::fromRfc4122(QByteArray::fromHex(m.args[0]));
	if(id.isNull()||m.args[1].isEmpty())return;
	if(m.args[1]==WLIOTProtocolDefs::hubDeviceIdentifiedMsg)
	{
		if(m.args.count()<3)return;
		onHubDeviceIdentified(id,m.args[2]);
	}
	else if(m.args[1]==WLIOTProtocolDefs::hubDeviceLostMsg)
	{
		if(!hubDevicesMap.contains(id))return;
		hubDevicesMap[id]->setSelfConnected(false);
		emit childDeviceLost(id);
	}
	else if(m.args[1]==WLIOTProtocolDefs::deviceInfoMsg)
	{
		if(m.args.count()<4)return;
		onHubDeviceIdentified(id,m.args[3]);
	}
	else
	{
		if(!hubDevicesMap.contains(id))return;
		Message m2=m;
		m2.title=m.args[1];
		m2.args.removeFirst();
		m2.args.removeFirst();
		hubDevicesMap[id]->onNewMessage(m2);
	}
}

void RealDevice::onHubDeviceIdentified(const QUuid &id,const QByteArray &name)
{
	if(!hubDevicesMap.contains(id))
	{
		hubDevicesMap[id]=0;
		HubDevice *dev=new HubDevice(id,name,this);
		hubDevicesMap[id]=dev;
		dev->setSelfConnected(true);
		connect(dev,&HubDevice::internalSyncFailed,this,&RealDevice::onChildDeviceSyncFailed);
		emit childDeviceIdentified(id);
	}
	else
	{
		HubDevice *dev=hubDevicesMap[id];
		if(dev->isConnected()&&dev->mSensorsLoaded&&dev->mControlsLoaded)
			return;
		dev->setSelfConnected(true);
		emit childDeviceIdentified(id);
	}
}

bool RealDevice::identifyHub()
{
	if(!isConnected()||devId.isNull())return false;
	CommandCall call(this,WLIOTProtocolDefs::identifyHubMsg,this);
	call.setUseCallMsg(false);
	call.setupTimer(WLIOTProtocolDefs::identifyWaitTime);
	return call.call();
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

bool RealDevice::isConnected()const
{
	return mConnected;
}

bool RealDevice::isIdentified()
{
	return mConnected&&!devId.isNull();
}

QUuid RealDevice::id()
{
	return devId;
}

QUuid RealDevice::typeId()
{
	return devTypeId;
}

QByteArray RealDevice::name()
{
	return devName;
}

void RealDevice::forceRename(const QByteArray &newName,bool silent)
{
	if(devName==newName)
		return;
	devName=newName;
	if(!silent)
		emit identificationChanged(devId,devId);
}

bool RealDevice::getSensorsDescription(QList<SensorDef> &sensors)
{
	if(mSensorsLoaded)
	{
		sensors=mSensors;
		return true;
	}
	CommandCall call(this,WLIOTProtocolDefs::getSensorsCommand);
	if(!call.call())return false;
	QByteArrayList retVal=call.returnValue();
	if(retVal.count()<1)return false;
	retVal[0]=retVal[0].trimmed();
	QList<SensorDef> newSensors;
	if(retVal[0].startsWith('{'))
	{
		if(!SensorDef::parseJsonDescription(retVal[0],newSensors))return false;
	}
	else if(!SensorDef::parseXmlDescription(retVal[0],newSensors))return false;
	mSensors=newSensors;
	mSensorsLoaded=true;
	sensors=mSensors;
	return true;
}

bool RealDevice::getControlsDescription(ControlsGroup &controls)
{
	if(mControlsLoaded)
	{
		controls=mControls;
		return true;
	}
	CommandCall call(this,WLIOTProtocolDefs::getControlsCommand);
	if(!call.call())return false;
	QByteArrayList retVal=call.returnValue();
	if(retVal.count()<1)return false;
	retVal[0]=retVal[0].trimmed();
	ControlsGroup newControls;
	if(retVal[0].startsWith('{'))
	{
		if(!ControlsGroup::parseJsonDescription(retVal[0],newControls))return false;
	}
	else if(!ControlsGroup::parseXmlDescription(retVal[0],newControls))return false;
	mControls=newControls;
	mControlsLoaded=true;
	controls=mControls;
	return true;
}

bool RealDevice::getState(DeviceState &state)
{
	if(mStateLoaded)
	{
		state=mState;
		return true;
	}
	CommandCall call(this,WLIOTProtocolDefs::getStateCommand);
	if(!call.call())return false;
	QByteArrayList retVal=call.returnValue();
	if(!mState.parseMsgArgs(retVal))return false;
	mStateLoaded=true;
	state=mState;
	return true;
}

bool RealDevice::isHubDevice()
{
	return hubDevice;
}

QList<QUuid> RealDevice::childDevices()
{
	return hubDevicesMap.keys();
}

RealDevice* RealDevice::childDevice(const QUuid &id)
{
	return hubDevicesMap.value(id,0);
}
