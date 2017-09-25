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

#include "JSDevice.h"
#include "ARpcBase/ARpcSyncCall.h"
#include <QDateTime>

JSDevice::JSDevice(ARpcRealDevice *d,QScriptEngine *e,QObject *parent)
	:QObject(parent)
{
	dev=d;
	js=e;
	connect(dev,&ARpcRealDevice::identificationChanged,this,&JSDevice::identificationChanged);
}

bool JSDevice::isIdentified()
{
	return dev->isIdentified();
}

QString JSDevice::id()
{
	return dev->id().toString();
}

QString JSDevice::name()
{
	return dev->name();
}

QScriptValue JSDevice::getSensorsDescription()
{
	QList<ARpcSensor> sensors;
	if(!dev->getSensorsDescription(sensors))
		return js->nullValue();
	QScriptValue arr=js->newArray(sensors.count());
	for(int i=0;i<sensors.count();++i)
	{
		ARpcSensor &s=sensors[i];
		QScriptValue sObj=js->newObject();
		sObj.setProperty("name",s.name);
		sObj.setProperty("type",ARpcSensor::typeToString(s.type));
		QScriptValue cObj=js->newObject();
		cObj=js->newVariant(cObj,s.constraints);
		sObj.setProperty("constraints",cObj);
		arr.setProperty(i,sObj);
	}
	return arr;
}

QScriptValue JSDevice::sendCommand(QScriptValue cmd,QScriptValue args)
{
	if(!cmd.isString()||!args.isArray()||cmd.toString().isEmpty())
		return js->nullValue();
	QStringList strArgs=arrayToStringList(args);
	ARpcSyncCall call;
	QStringList retVal;
	bool ok=call.call(dev,cmd.toString(),strArgs,retVal);
	QScriptValue obj=js->newObject();
	obj.setProperty("ok",ok);
	QScriptValue arr=js->newArray(retVal.count());
	for(int i=0;i<retVal.count();++i)
		arr.setProperty(i,retVal[i]);
	obj.setProperty("value",arr);
	return obj;
}

bool JSDevice::valToString(const QScriptValue &val,QString &str)
{
	if(val.isString())
		str=val.toString();
	else if(val.isBool())
		str=val.toBool()?"1":"0";
	else if(val.isDate())
		str=QString::number(val.toDateTime().toMSecsSinceEpoch());
	else if(val.isNumber())
		str=QString::number(val.toNumber());
	else if(val.isVariant()&&val.toVariant().canConvert(QVariant::String))
		str=val.toVariant().toString();
	else
		return false;
	return true;
}

QStringList JSDevice::arrayToStringList(QScriptValue arr)
{
	QStringList retVal;
	quint32 i=0;
	while(true)
	{
		QScriptValue v=arr.property(i);
		if(!v.isValid()||v.isNull()||v.isUndefined())
			break;
		QString str;
		if(valToString(v,str))
			retVal.append(str);
		++i;
	}
	return retVal;
}

QScriptValue JSDevice::stringListToArray(const QStringList &list)
{
	QScriptValue retVal=js->newArray(list.count());
	for(int i=0;i<list.count();++i)
		retVal.setProperty(i,list[i]);
	return retVal;
}
