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

#include "wliot/devices/VirtualDevice.h"
#include "wliot/WLIOTConfig.h"

VirtualDevice::VirtualDevice(const QUuid &id,const QByteArray &name,const QList<SensorDef> &sensors,
	const ControlsGroup &controls,QObject *parent)
	:RealDevice(parent)
{
	mId=id;
	mName=name;
	SensorDef::dumpToXml(sensorsXml,sensors);
	mSensors=sensors;
	ControlsGroup::dumpToXml(controlsXml,controls);
	mControls=controls;
	prepareStateFromControls(mControls);
	resetIdentification(mId,mName);
}

void VirtualDevice::setupAdditionalStateAttributes(const QByteArrayList &names)
{
	selfState.additionalAttributes.clear();
	for(const auto &n:names)
		selfState.additionalAttributes[n]=QByteArray();
}

bool VirtualDevice::writeMsgToDevice(const Message &m)
{
	return QMetaObject::invokeMethod(this,"writeMsgToDeviceQueued",Qt::QueuedConnection,Q_ARG(Message,m));
}

void VirtualDevice::setConnected(bool c)
{
	if(c)
		onConnected();
	else onDisconnected();
}

void VirtualDevice::writeMsgToDeviceQueued(Message m)
{
	if(m.title==WLIOTConfig::identifyMsg)
		onNewMessage(Message(WLIOTConfig::deviceInfoMsg,QByteArrayList()<<devId.toByteArray()<<devName));
	else if(m.title==WLIOTConfig::devSyncMsg)
		emit syncMsgNotify();
	else if(m.title==WLIOTConfig::funcCallMsg)
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
			if(m.args[1]=="#sensors")
				writeOk(QByteArrayList()<<sensorsXml);
			else if(m.args[1]=="#controls")
				writeOk(QByteArrayList()<<controlsXml);
			else if(m.args[1]=="#state")
				writeOk(selfState.dumpToMsgArgs());
		}
		else
		{
			bool ok=false;
			QByteArray cmd=m.args[1];
			QByteArrayList retVal;
			emit processDeviceCommand(cmd,m.args.mid(2),ok,retVal);
			if(ok)
				writeOk(retVal);
			else
				writeErr(retVal);
		}
	}
	else
		writeInfo(QByteArrayList()<<"ERROR: unknown message");
}

void VirtualDevice::writeOk(const QByteArrayList &args)
{
	onNewMessage({WLIOTConfig::funcAnswerOkMsg,QByteArrayList()<<callIdStr<<args});
}

void VirtualDevice::writeErr(const QByteArrayList &args)
{
	onNewMessage({WLIOTConfig::funcAnswerErrMsg,QByteArrayList()<<callIdStr<<args});
}

void VirtualDevice::prepareStateFromControls(const ControlsGroup &grp)
{
	for(int i=0;i<grp.elements.count();++i)
	{
		const ControlsGroup::Element &elem=grp.elements[i];
		if(elem.isGroup())
			prepareStateFromControls(*elem.group());
		else
		{
			const CommandControl *ctl=elem.control();
			auto &paramsMap=selfState.commandParams[ctl->command];
			for(int i=0;i<ctl->params.count();++i)
				paramsMap[i+1]=QByteArray();
		}
	}
}

void VirtualDevice::writeInfo(const QByteArrayList &args)
{
	onNewMessage({WLIOTConfig::infoMsg,args});
}

void VirtualDevice::writeMeasurement(const QByteArray &name,const QByteArrayList &values)
{
	onNewMessage({WLIOTConfig::measurementMsg,QByteArrayList()<<name<<values});
}

void VirtualDevice::commandParamStateChanged(const QByteArray &name,int paramNum,const QByteArray &value)
{
	if(!selfState.commandParams.contains(name))return;
	if(!selfState.commandParams[name].contains(paramNum))return;
	selfState.commandParams[name][paramNum]=value;
	onNewMessage(Message(WLIOTConfig::stateChangedMsg,QByteArrayList()<<name<<QByteArray::number(paramNum)<<value));
}

void VirtualDevice::additionalStateChanged(const QByteArray &name,const QByteArray &value)
{
	if(!selfState.additionalAttributes.contains(name))return;
	selfState.additionalAttributes[name]=value;
	onNewMessage(Message(WLIOTConfig::stateChangedMsg,QByteArrayList()<<"#"<<name<<value));
}

void VirtualDevice::writeCommandCallSync(const QByteArray &callId)
{
	onNewMessage(Message(WLIOTConfig::funcSynccMsg,QByteArrayList()<<callId));
}
