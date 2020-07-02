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
#include "wliot/WLIOTProtocolDefs.h"

using namespace WLIOT;

JSVirtualDevice::JSVirtualDevice(RealDevice *d,
	QScriptEngine *e,const QList<SensorDef> &sensors,
	const ControlsGroup &controls,QObject *parent)
	:JSDevice(d,e,parent)
{
	mBackend=0;
	cmdCallback=js->nullValue();
	mSensors=sensors;
	mControls=controls;
	prepareStateFromControls(mControls);
}

JSVirtualDevice::~JSVirtualDevice()
{
	if(mBackend)
		mBackend->forceDisconnect();
}

void JSVirtualDevice::setBackend(VirtualDeviceBackend *be)
{
	mBackend=be;
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
	if(!argsList.isArray()||!mBackend)return;
	mBackend->emulateMessageFromDevice(
		Message(WLIOTProtocolDefs::infoMsg,jsArrayToByteArrayList(argsList)));
}

void JSVirtualDevice::writeMeasurement(QScriptValue name,QScriptValue value)
{
	if(!name.isString()||value.isNull()||!mBackend)
		return;
	QByteArrayList measArgs=JSSensorValue::sensorValueFromJsObject(js,value);
	if(measArgs.isEmpty())return;
	mBackend->emulateMessageFromDevice(Message(WLIOTProtocolDefs::measurementMsg,
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
	quint32 paramIndexInt=paramIndex.toUInt32();
	QByteArray valueStr;
	if(value.isNumber())
		valueStr=QByteArray::number(value.toNumber(),'g',200);
	else valueStr=value.toString().toUtf8();
	if(!mState.commandParams.contains(cmdStr))return;
	auto &pMap=mState.commandParams[cmdStr];
	if(!pMap.contains(paramIndexInt))return;
	pMap[paramIndexInt]=valueStr;
	if(mBackend)mBackend->emulateMessageFromDevice(Message(
		WLIOTProtocolDefs::stateChangedMsg,QByteArrayList()<<cmdStr<<QByteArray::number(paramIndexInt)<<valueStr));
}

void JSVirtualDevice::additionalStateChanged(const QScriptValue &paramName,const QScriptValue &value)
{
	if(!paramName.isString()||(!value.isString()&&!value.isNumber()))
		return;
	QByteArray paramNameStr=paramName.toString().toUtf8();
	QByteArray valueStr;
	if(value.isNumber())
		valueStr=QByteArray::number(value.toNumber(),'g',200);
	else valueStr=value.toString().toUtf8();
	if(!mState.additionalAttributes.contains(paramNameStr))return;
	mState.additionalAttributes[paramNameStr]=valueStr;
	if(mBackend)mBackend->emulateMessageFromDevice(Message(
		WLIOTProtocolDefs::stateChangedMsg,QByteArrayList()<<"#"<<paramNameStr<<valueStr));
}

void JSVirtualDevice::onMessageToVDev(VirtualDeviceBackend *vDev,const Message &m)
{
	if(vDev!=mBackend)return;
	if(m.title==WLIOTProtocolDefs::funcCallMsg)
	{
		QByteArray callIdStr;
		if(m.args.count()<2||m.args[0].size()==0||m.args[1].size()==0)
			return;
		callIdStr=m.args[0];
		if(m.args[1][0]=='#')
		{
			if(m.args[1]==WLIOTProtocolDefs::getSensorsCommand)
			{
				QByteArray data;
				SensorDef::dumpToXml(data,mSensors);
				writeOk(callIdStr,QByteArrayList()<<data);
			}
			else if(m.args[1]==WLIOTProtocolDefs::getControlsCommand)
			{
				QByteArray data;
				ControlsGroup::dumpToXml(data,mControls);
				writeOk(callIdStr,QByteArrayList()<<data);
			}
			else if(m.args[1]==WLIOTProtocolDefs::getStateCommand)
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
	if(mBackend)mBackend->emulateMessageFromDevice(Message(WLIOTProtocolDefs::funcAnswerOkMsg,
		QByteArrayList()<<callId<<args));
}

void JSVirtualDevice::writeErr(const QByteArray &callId, const QByteArrayList &args)
{
	if(mBackend)mBackend->emulateMessageFromDevice(Message(WLIOTProtocolDefs::funcAnswerErrMsg,
		QByteArrayList()<<callId<<args));
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
