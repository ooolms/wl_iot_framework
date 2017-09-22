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

JSVirtualDevice::JSVirtualDevice(ARpcVirtualDevice *d,QScriptEngine *e,QObject *parent)
	:JSDevice(d,e,parent)
{
	vDev=d;
	cmdCallback=js->nullValue();
	connect(vDev,&ARpcVirtualDevice::processDeviceCommand,this,&JSVirtualDevice::onProcessDeviceCommand);
}

void JSVirtualDevice::writeMsgFromDevice(QScriptValue titleStr,QScriptValue argsArray)
{
	if(!titleStr.isString()||!argsArray.isArray())
		return;
	QString title=titleStr.toString();
	QStringList args=arrayToStringList(argsArray);
	vDev->writeMsgFromDevice(ARpcMessage(title,args));
}

bool JSVirtualDevice::setSensors(QScriptValue sensorsXml)
{
	QList<ARpcSensor> sensors;
	if(sensorsXml.isString()&&ARpcSensor::parseXmlDescription(sensorsXml.toString(),sensors))
	{
		vDev->setSensors(sensors);
		return true;
	}
	return false;
}

bool JSVirtualDevice::setControls(QScriptValue controlsXml)
{
	ARpcControlsGroup controls;
	if(controlsXml.isString()&&ARpcControlsGroup::parseXmlDescription(controlsXml.toString(),controls))
	{
		vDev->setControls(controls);
		return true;
	}
	return false;
}

void JSVirtualDevice::writeInfo(QScriptValue argsList)
{
	if(!argsList.isArray())
		return;
	vDev->writeInfo(arrayToStringList(argsList));
}

void JSVirtualDevice::writeMeasurement(QScriptValue name,QScriptValue values)
{
	if(!name.isString()||!values.isArray())
		return;
	vDev->writeMeasurement(name.toString(),arrayToStringList(values));
}

void JSVirtualDevice::writeSync()
{
	vDev->writeSync();
}

void JSVirtualDevice::setCommandCallback(QScriptValue cbFunc)
{
	cmdCallback=cbFunc;
}

void JSVirtualDevice::onProcessDeviceCommand(const QString &cmd,const QStringList &args,bool &ok,QStringList &retVal)
{
	ok=false;
	if(cmdCallback.isFunction())
	{
		QScriptValueList scriptArgs;
		scriptArgs.append(cmd);
		scriptArgs.append(stringListToArray(args));
		QScriptValue val=cmdCallback.call(QScriptValue(),scriptArgs);
		if(val.isArray())
		{
			ok=true;
			retVal=arrayToStringList(val);
		}
	}
}
