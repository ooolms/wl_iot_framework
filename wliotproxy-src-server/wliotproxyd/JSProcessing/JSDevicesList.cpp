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
#include "../MainServerConfig.h"

using namespace WLIOT;

JSDevicesList::JSDevicesList(QScriptEngine *e,IdType uid)
	:QObject(e)
{
	js=e;
	ownerUid=uid;
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
	{
		if(!MainServerConfig::accessManager.userCanAccessDevice(
			ids[i],ownerUid,DevicePolicyActionFlag::READ_STORAGES))
				continue;
		retVal.setProperty(i,ids[i].toString());
	}
	return retVal;
}

QScriptValue JSDevicesList::device(QScriptValue idOrNameStr)
{
	if(!idOrNameStr.isString())
		return js->nullValue();
	RealDevice *dev=ServerInstance::inst().devices()->deviceByIdOrName(idOrNameStr.toString().toUtf8());
	if(!dev)
		return js->nullValue();
	if(!MainServerConfig::accessManager.userCanAccessDevice(dev->id(),ownerUid,DevicePolicyActionFlag::ANY))
		return js->nullValue();
	JSDevice *jsDev=new JSDevice(dev,js,ownerUid);
	return js->newQObject(jsDev,QScriptEngine::ScriptOwnership);
}

QScriptValue JSDevicesList::registerVirtualDevice(QScriptValue idStr,QScriptValue nameStr,
	QScriptValue typeIdStr,QScriptValue sensorsXml)
{
	return registerVirtualDevice(idStr,nameStr,typeIdStr,sensorsXml,QScriptValue(js,""));
}

QScriptValue JSDevicesList::registerVirtualDevice(QScriptValue idStr,QScriptValue nameStr,QScriptValue typeIdStr,
	QScriptValue sensorsVar,QScriptValue controlsVar)
{
	if(!idStr.isString()||!nameStr.isString())
		return js->nullValue();
	QUuid id(idStr.toString());
	QByteArray name=nameStr.toString().toUtf8();
	if(id.isNull()||name.isEmpty())
		return js->nullValue();
	AccessMgr &apm=MainServerConfig::accessManager;
	if(!apm.userCanRegisterVirtualDevice(id,ownerUid))
		return js->nullValue();
	QUuid typeId(typeIdStr.toString());
	QList<SensorDef> sensors;
	ControlsGroup controls;
	QByteArray sensorsStr=sensorsVar.toString().toUtf8();
	QByteArray controlsStr=controlsVar.toString().toUtf8();
	bool ok=false;
	if(sensorsStr.startsWith("{"))
		ok=SensorsParser::parseJsonDescription(sensorsStr,sensors);
	else ok=SensorsParser::parseXmlDescription(sensorsStr,sensors);
	if(!ok)return js->nullValue();
	if(controlsStr.startsWith("{"))
		ControlsParser::parseJsonDescription(controlsStr,controls);
	else ControlsParser::parseXmlDescription(controlsStr,controls);
	IdType devOwner=apm.devOwner(id);
	if(devOwner==nullId&&!apm.setDevOwner(id,ownerUid))
		return js->nullValue();
	JSVirtualDevice *jsVDev=new JSVirtualDevice(
		ServerInstance::inst().devices()->deviceById(id),js,sensors,controls,ownerUid);
	VirtualDeviceBackend *be=new VirtualDeviceBackend(
		id,name,typeId,static_cast<WLIOT::IVirtualDeviceBackendCallback*>(jsVDev));
	jsVDev->setBackend(be);
	RealDevice *dev=ServerInstance::inst().devices()->registerVirtualDevice(be);
	if(!dev||dev!=jsVDev->device())//paranoid mode
	{
		delete jsVDev;
		return js->nullValue();
	}
	return js->newQObject(jsVDev,QScriptEngine::ScriptOwnership);
}

void JSDevicesList::onDeviceIdentified(QUuid id,QByteArray name,QUuid typeId)
{
	if(!MainServerConfig::accessManager.userCanAccessDevice(id,ownerUid,DevicePolicyActionFlag::ANY))
		return;
	if(typeId.isNull())
		emit deviceIdentified(js->toScriptValue(id.toString()),
			js->toScriptValue(QString::fromUtf8(name)),js->nullValue());
	else emit deviceIdentified(js->toScriptValue(id.toString()),
		js->toScriptValue(QString::fromUtf8(name)),js->toScriptValue(typeId.toByteArray()));
}

void JSDevicesList::onDeviceDisconnected(QUuid id)
{
	if(!MainServerConfig::accessManager.userCanAccessDevice(id,ownerUid,DevicePolicyActionFlag::ANY))
		return;
	emit deviceDisconnected(js->toScriptValue(id.toString()));
}
