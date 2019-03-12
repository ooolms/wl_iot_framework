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

#include "JSVirtualDevice.h"
#include "JSSensorValue.h"
#include "wliot/WLIOTConfig.h"

JSVirtualDevice::JSVirtualDevice(VirtualDevice *d,QScriptEngine *e,const QList<SensorDef> &sensors,
	const ControlsGroup &controls,QObject *parent)
	:JSDevice(d,e,parent)
{
	vDev=d;
	vDev->setClientPtr(this);
	cmdCallback=js->nullValue();
	mSensors=sensors;
	mControls=controls;
	connect(vDev,&VirtualDevice::messageToDevice,this,
		&JSVirtualDevice::onMessageToDevice,Qt::DirectConnection);
	prepareStateFromControls(mControls);
}

JSVirtualDevice::~JSVirtualDevice()
{
	vDev->setClientPtr(0);
}

void JSVirtualDevice::setupAdditionalStateAttributes(const QScriptValue &names)
{
	if(!names.isArray())return;
	QByteArrayList namesList=jsArrayToByteArrayList(names);
	mState.additionalAttributes.clear();
	for(QByteArray &n:namesList)
		mState.additionalAttributes[n]="";
}

void JSVirtualDevice::writeInfo(QScriptValue argsList)
{
	if(!argsList.isArray())return;
	vDev->onMessageFromDevice(Message(WLIOTConfig::infoMsg,jsArrayToByteArrayList(argsList)));
}

void JSVirtualDevice::writeMeasurement(QScriptValue name,QScriptValue value)
{
	if(!name.isString()||value.isNull())
		return;
	QByteArrayList measArgs=JSSensorValue::sensorValueFromJsObject(js,value);
	if(!measArgs.isEmpty())
		vDev->onMessageFromDevice(Message(WLIOTConfig::measurementMsg,
			QByteArrayList()<<name.toString().toUtf8()<<measArgs));
}

void JSVirtualDevice::setCommandCallback(QScriptValue cbFunc)
{
	cmdCallback=cbFunc;
}

void JSVirtualDevice::commandParamStateChanged(
	const QScriptValue &cmd,QScriptValue paramIndex,const QScriptValue &value)
{
	if(!cmd.isString()||!paramIndex.isNumber()||(!value.isString()&&!value.isNumber()))
		return;
	QByteArray cmdStr=cmd.toString().toUtf8();
	int paramIndexInt=paramIndex.toInt32();
	QByteArray valueStr;
	if(value.isNumber())
		valueStr=QByteArray::number(value.toNumber());
	else valueStr=value.toString().toUtf8();
	if(!mState.commandParams.contains(cmdStr))return;
	auto &pMap=mState.commandParams[cmdStr];
	if(!pMap.contains(paramIndexInt))return;
	pMap[paramIndexInt]=valueStr;
	vDev->onMessageFromDevice(Message(
		WLIOTConfig::stateChangedMsg,QByteArrayList()<<cmdStr<<QByteArray::number(paramIndexInt)<<valueStr));
}

void JSVirtualDevice::additionalStateChanged(const QScriptValue &paramName,const QScriptValue &value)
{
	if(!paramName.isString()||(!value.isString()&&!value.isNumber()))
		return;
	QByteArray paramNameStr=paramName.toString().toUtf8();
	QByteArray valueStr;
	if(value.isNumber())
		valueStr=QByteArray::number(value.toNumber());
	else valueStr=value.toString().toUtf8();
	if(!mState.additionalAttributes.contains(paramNameStr))return;
	mState.additionalAttributes[paramNameStr]=valueStr;
	vDev->onMessageFromDevice(Message(WLIOTConfig::stateChangedMsg,QByteArrayList()<<"#"<<paramNameStr<<valueStr));
}

void JSVirtualDevice::onMessageToDevice(const Message &m)
{
	if(m.title==WLIOTConfig::identifyMsg)
	{
		vDev->onMessageFromDevice(Message(WLIOTConfig::deviceInfoMsg,
			QByteArrayList()<<vDev->id().toByteArray()<<vDev->name()));
	}
	else if(m.title==WLIOTConfig::devSyncMsg)
	{
		vDev->onMessageFromDevice(WLIOTConfig::devSyncrMsg);
	}
	else if(m.title==WLIOTConfig::funcCallMsg)
	{
		QByteArray callIdStr;
		if(m.args.count()<2||m.args[0].size()==0||m.args[1].size()==0)
			return;
		callIdStr=m.args[0];
		if(m.args[1][0]=='#')
		{
			if(m.args[1]==WLIOTConfig::getSensorsCommand)
			{
				QByteArray data;
				SensorDef::dumpToXml(data,mSensors);
				writeOk(callIdStr,QByteArrayList()<<data);
			}
			else if(m.args[1]==WLIOTConfig::getControlsCommand)
			{
				QByteArray data;
				ControlsGroup::dumpToXml(data,mControls);
				writeOk(callIdStr,QByteArrayList()<<data);
			}
			else if(m.args[1]==WLIOTConfig::getStateCommand)
				writeOk(callIdStr,mState.dumpToMsgArgs());
			else writeErr(callIdStr,QByteArrayList()<<"bad system command");
		}
		else
		{
			QByteArrayList retVal;
			bool ok=processCommand(m.args[1],m.args.mid(2),retVal);
			if(ok)
				writeOk(callIdStr,retVal);
			else writeErr(callIdStr,retVal);
		}
	}
}

void JSVirtualDevice::writeOk(const QByteArray &callId,const QByteArrayList &args)
{
	vDev->onMessageFromDevice(Message(WLIOTConfig::funcAnswerOkMsg,QByteArrayList()<<callId<<args));
}

void JSVirtualDevice::writeErr(const QByteArray &callId, const QByteArrayList &args)
{
	vDev->onMessageFromDevice(Message(WLIOTConfig::funcAnswerErrMsg,QByteArrayList()<<callId<<args));
}

bool JSVirtualDevice::processCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)
{
	bool ok=false;
	if(cmdCallback.isFunction())
	{
		QScriptValueList scriptArgs;
		scriptArgs.append(QString::fromUtf8(cmd));
		scriptArgs.append(byteArrayListToJsArray(args));
		QScriptValue val=cmdCallback.call(js->globalObject(),scriptArgs);
		if(val.isArray())
		{
			ok=true;
			retVal=jsArrayToByteArrayList(val);
		}
		else if(val.isObject())
		{
			ok=val.property("done").toBool();
			QScriptValue result=val.property("result");
			if(result.isArray())
				retVal=jsArrayToByteArrayList(result);
			else retVal.append(result.toString().toUtf8());
		}
		else if(val.isBool())
			ok=val.toBool();
	}
	else retVal.append("unknown command");
	return ok;
}

void JSVirtualDevice::prepareStateFromControls(const ControlsGroup &grp)
{
	for(int i=0;i<grp.elements.count();++i)
	{
		const ControlsGroup::Element &elem=grp.elements[i];
		if(elem.isGroup())
			prepareStateFromControls(*elem.group());
		else
		{
			const CommandControl *ctl=elem.control();
			auto &paramsMap=mState.commandParams[ctl->command];
			for(int i=0;i<ctl->params.count();++i)
				paramsMap[i+1]=QByteArray();
		}
	}
}
