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

#include "JSDevicesList.h"
#include "JSDevice.h"
#include "JSVirtualDevice.h"
#include "../ServerInstance.h"

JSDevicesList::JSDevicesList(QScriptEngine *e,QObject *parent)
	:QObject(parent)
{
	js=e;
	connect(ServerInstance::inst().devices(),&Devices::deviceIdentified,
		this,&JSDevicesList::onDeviceIdentified);
	connect(ServerInstance::inst().devices(),&Devices::deviceDisconnected,
		this,&JSDevicesList::onDeviceDisconnected);
}

QScriptValue JSDevicesList::devices()
{
	QList<QUuid> ids=ServerInstance::inst().devices()->identifiedDevicesIds();
	QScriptValue retVal=js->newArray(ids.count());
	for(int i=0;i<ids.count();++i)
		retVal.setProperty(i,ids[i].toString());
	return retVal;
}

QScriptValue JSDevicesList::device(QScriptValue idOrNameStr)
{
	if(!idOrNameStr.isString())
		return js->nullValue();
	RealDevice *dev=ServerInstance::inst().devices()->deviceByIdOrName(idOrNameStr.toString().toUtf8());
	if(!dev)
		return js->nullValue();
	JSDevice *jsDev=new JSDevice(dev,js);
	return js->newQObject(jsDev,QScriptEngine::ScriptOwnership);
}

QScriptValue JSDevicesList::registerVirtualDevice(QScriptValue idStr,QScriptValue nameStr,QScriptValue sensorsXml)
{
	return registerVirtualDevice(idStr,nameStr,sensorsXml,QScriptValue(js,""));
}

QScriptValue JSDevicesList::registerVirtualDevice(QScriptValue idStr,QScriptValue nameStr,QScriptValue sensorsVar,
	QScriptValue controlsVar)
{
	if(!idStr.isString()||!nameStr.isString())
		return js->nullValue();
	QUuid id(idStr.toString());
	QByteArray name=nameStr.toString().toUtf8();
	if(id.isNull()||name.isEmpty())
		return js->nullValue();
	QList<SensorDef> sensors;
	ControlsGroup controls;
	QByteArray sensorsStr=sensorsVar.toString().toUtf8();
	QByteArray controlsStr=controlsVar.toString().toUtf8();
	bool ok=false;
	if(sensorsStr.startsWith("{"))
		ok=SensorDef::parseJsonDescription(sensorsStr,sensors);
	else ok=SensorDef::parseXmlDescription(sensorsStr,sensors);
	if(!ok)return js->nullValue();
	if(controlsStr.startsWith("{"))
		ControlsGroup::parseJsonDescription(controlsStr,controls);
	else ControlsGroup::parseXmlDescription(controlsStr,controls);
	VirtualDevice *dev=ServerInstance::inst().devices()->registerVirtualDevice(id,name);
	if(!dev||dev->clientPtr())
		return js->nullValue();
	return js->newQObject(new JSVirtualDevice(dev,js,sensors,controls),QScriptEngine::ScriptOwnership);
}

void JSDevicesList::onDeviceIdentified(QUuid id,QByteArray name,QByteArray type)
{
	emit deviceIdentified(js->toScriptValue(id.toString()),
		js->toScriptValue(QString::fromUtf8(name)),js->toScriptValue(QString::fromUtf8(type)));
}

void JSDevicesList::onDeviceDisconnected(QUuid id)
{
	emit deviceDisconnected(js->toScriptValue(id.toString()));
}
