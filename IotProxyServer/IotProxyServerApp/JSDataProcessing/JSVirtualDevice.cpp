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

JSVirtualDevice::JSVirtualDevice(ARpcVirtualDevice *d,QScriptEngine *e,QObject *parent)
	:JSDevice(d,e,parent)
{
	vDev=d;
	cmdCallback=js->nullValue();
	connect(vDev,&ARpcVirtualDevice::processDeviceCommand,this,
		&JSVirtualDevice::onProcessDeviceCommand,Qt::DirectConnection);
	connect(vDev,&ARpcVirtualDevice::syncMsgNotify,this,
		&JSVirtualDevice::syncMsgNotify,Qt::DirectConnection);
}

void JSVirtualDevice::writeMsgFromDevice(QScriptValue titleStr,QScriptValue argsArray)
{
	if(!titleStr.isString()||!argsArray.isArray())
		return;
	QByteArray title=titleStr.toString().toUtf8();
	QByteArrayList args=jsArrayToByteArrayList(argsArray);
	vDev->writeMsgFromDevice(ARpcMessage(title,args));
}

void JSVirtualDevice::writeInfo(QScriptValue argsList)
{
	if(!argsList.isArray())
		return;
	vDev->writeInfo(jsArrayToByteArrayList(argsList));
}

void JSVirtualDevice::writeMeasurement(QScriptValue name,QScriptValue value)
{
	QScopedPointer<ARpcSensorValue>val(JSSensorValue::sensorValueFromJsObject(js,value));
	if(!name.isString()||val.isNull())
		return;
	vDev->writeMeasurement(name.toString().toUtf8(),val->dumpToMsgArgs());
}

void JSVirtualDevice::setCommandCallback(QScriptValue cbFunc)
{
	cmdCallback=cbFunc;
}

void JSVirtualDevice::reconnect()
{
	vDev->reconnect();
}

void JSVirtualDevice::onProcessDeviceCommand(const QByteArray &cmd,const QByteArrayList &args,
	bool &ok,QByteArrayList &retVal)
{
	ok=false;
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
			QScriptValue val=val.property("value");
			if(val.isArray())
				retVal=jsArrayToByteArrayList(val);
			else retVal.append(val.toString().toUtf8());
		}
		else if(val.isBool())
			ok=val.toBool();
	}
	else retVal.append("unknown command");
}
