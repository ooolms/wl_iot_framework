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
	prepareStateFromControls(mControls);
	resetIdentification(mId,mName);
}

void ARpcVirtualDevice::reconnect()
{
	emit identificationChanged(mId,mId);
}

void ARpcVirtualDevice::setupAdditionalStateAttributes(const QByteArrayList &names)
{
	selfState.additionalAttributes.clear();
	for(const auto &n:names)
		selfState.additionalAttributes[n]=QByteArray();
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

bool ARpcVirtualDevice::isConnected()
{
	return true;
}

void ARpcVirtualDevice::writeMsgFromDevice(const ARpcMessage &m)
{
	emit newMessage(m);
}

void ARpcVirtualDevice::writeOk(const QByteArrayList &args)
{
	writeMsgFromDevice({ARpcConfig::funcAnswerOkMsg,QByteArrayList()<<callIdStr<<args});
}

void ARpcVirtualDevice::writeErr(const QByteArrayList &args)
{
	writeMsgFromDevice({ARpcConfig::funcAnswerErrMsg,QByteArrayList()<<callIdStr<<args});
}

void ARpcVirtualDevice::prepareStateFromControls(const ARpcControlsGroup &grp)
{
	for(int i=0;i<grp.elements.count();++i)
	{
		const ARpcControlsGroup::Element &elem=grp.elements[i];
		if(elem.isGroup())
			prepareStateFromControls(*elem.group());
		else
		{
			const ARpcCommandControl *ctl=elem.control();
			auto &paramsMap=selfState.commandParams[ctl->command];
			for(int i=0;i<ctl->params.count();++i)
				paramsMap[i+1]=QByteArray();
		}
	}
}

void ARpcVirtualDevice::writeInfo(const QByteArrayList &args)
{
	writeMsgFromDevice({ARpcConfig::infoMsg,args});
}

void ARpcVirtualDevice::writeMeasurement(const QByteArray &name,const QByteArrayList &values)
{
	writeMsgFromDevice({ARpcConfig::measurementMsg,QByteArrayList()<<name<<values});
}
