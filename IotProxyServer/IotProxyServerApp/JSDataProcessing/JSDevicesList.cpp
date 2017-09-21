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
#include "../IotProxyInstance.h"

JSDevicesList::JSDevicesList(QScriptEngine *e,QObject *parent)
	:QObject(parent)
{
	js=e;
}

QScriptValue JSDevicesList::devices()
{
	QList<QUuid> ids=IotProxyInstance::inst().identifiedDevicesIds();
	QScriptValue retVal=js->newArray(ids.count());
	for(int i=0;i<ids.count();++i)
		retVal.setProperty(i,ids[i].toString());
	return retVal;
}

QScriptValue JSDevicesList::device(QScriptValue idStr)
{
	if(!idStr.isString())
		return js->nullValue();
	QUuid id(idStr.toString());
	if(id.isNull())
		return js->nullValue();
	ARpcRealDevice *dev=IotProxyInstance::inst().deviceById(id);
	if(!dev)
		return js->nullValue();
	JSDevice *jsDev=new JSDevice(dev,js);
	return js->newQObject(jsDev,QScriptEngine::ScriptOwnership);
}

QScriptValue JSDevicesList::registerVirtualDevice(QScriptValue idStr,QScriptValue nameStr)
{
	if(!idStr.isString()||!nameStr.isString())
		return js->nullValue();
	QUuid id(idStr.toString());
	QString name=nameStr.toString();
	if(id.isNull()||name.isEmpty())
		return js->nullValue();
	ARpcVirtualDevice *dev=IotProxyInstance::inst().registerVirtualDevice(id,name);
	if(!dev)
		return js->nullValue();
	return js->newQObject(new JSVirtualDevice(dev,js),QScriptEngine::ScriptOwnership);
}
