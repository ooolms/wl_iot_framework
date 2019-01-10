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
#include <QScriptValueIterator>

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

QScriptValue JSDevice::getSensors()
{
	QList<ARpcSensorDef> sensors;
	if(!dev->getSensorsDescription(sensors))
		return js->nullValue();
	QScriptValue arr=js->newArray(sensors.count());
	for(int i=0;i<sensors.count();++i)
	{
		ARpcSensorDef &s=sensors[i];
		QScriptValue sObj=js->newObject();
		sObj.setProperty("name",QString::fromUtf8(s.name));
		sObj.setProperty("type",QString::fromUtf8(s.type.toString()));
		sObj.setProperty("title",QString::fromUtf8(s.title));
		sObj.setProperty("unit",QString::fromUtf8(s.unit));
		sObj.setProperty("constraints",byteArrayMapToJsObject(s.attributes));
		arr.setProperty(i,sObj);
	}
	return arr;
}

QScriptValue JSDevice::getCommandsList()
{
	ARpcControlsGroup controls;
	if(!dev->getControlsDescription(controls))
		return js->nullValue();
	QList<ARpcCommandControl> commands=controls.extractCommandsList();
	QScriptValue arr=js->newArray(commands.count());
	for(int i=0;i<commands.count();++i)
	{
		ARpcCommandControl &c=commands[i];
		QScriptValue paramsArr=js->newArray(c.params.count());
		for(int j=0;j<c.params.count();++j)
		{
			QScriptValue paramObj=js->newObject();
			paramObj.setProperty("type",js->toScriptValue(ARpcControlParam::typeToString(c.params[j].type)));
			for(auto it=c.params[j].attributes.begin();it!=c.params[j].attributes.end();++it)
				paramObj.setProperty(QString::fromUtf8(it.key()),js->toScriptValue(it.value()));
			paramsArr.setProperty(j,paramObj);
		}
		QScriptValue cmdObj=js->newObject();
		cmdObj.setProperty("command",js->toScriptValue(c.command));
		cmdObj.setProperty("params",paramsArr);
		arr.setProperty(i,cmdObj);
	}
	return arr;
}

QScriptValue JSDevice::sendCommand(QScriptValue cmd,QScriptValue args)
{
	if(!cmd.isString()||!args.isArray()||cmd.toString().isEmpty())
		return js->nullValue();
	QByteArrayList strArgs=jsArrayToByteArrayList(args);
	ARpcSyncCall call(dev,cmd.toString().toUtf8());
	call.setArgs(strArgs);
	bool ok=call.call();
	QByteArrayList retVal=call.returnValue();
	QScriptValue obj=js->newObject();
	obj.setProperty("ok",ok);
	QScriptValue arr=js->newArray(retVal.count());
	for(int i=0;i<retVal.count();++i)
		arr.setProperty(i,QString::fromUtf8(retVal[i]));
	obj.setProperty("value",arr);
	return obj;
}

bool JSDevice::valToByteArray(const QScriptValue &val,QByteArray &str)
{
	if(val.isString())
		str=val.toString().toUtf8();
	else if(val.isBool())
		str=val.toBool()?"1":"0";
	else if(val.isDate())
		str=QByteArray::number(val.toDateTime().toMSecsSinceEpoch());
	else if(val.isNumber())
		str=QByteArray::number(val.toNumber());
	else if(val.isVariant()&&val.toVariant().canConvert(QVariant::String))
		str=val.toVariant().toString().toUtf8();
	else
		return false;
	return true;
}

QByteArrayList JSDevice::jsArrayToByteArrayList(QScriptValue arr)
{
	QByteArrayList retVal;
	quint32 i=0;
	while(true)
	{
		QScriptValue v=arr.property(i);
		if(!v.isValid()||v.isNull()||v.isUndefined())
			break;
		QByteArray str;
		if(valToByteArray(v,str))
			retVal.append(str);
		++i;
	}
	return retVal;
}

QScriptValue JSDevice::byteArrayListToJsArray(const QByteArrayList &list)
{
	QScriptValue retVal=js->newArray(list.count());
	for(int i=0;i<list.count();++i)
		retVal.setProperty(i,QString::fromUtf8(list[i]));
	return retVal;
}

QMap<QByteArray,QByteArray> JSDevice::jsObjectToByteArrayMap(QScriptValue obj)
{
	QMap<QByteArray,QByteArray> retVal;
	QScriptValueIterator it(obj);
	while(it.hasNext())
	{
		it.next();
		QScriptValue val=it.value();
		QByteArray valBA;
		if(valToByteArray(val,valBA))
			retVal[it.name().toUtf8()]=valBA;
	}
	return retVal;
}

QScriptValue JSDevice::byteArrayMapToJsObject(const QMap<QByteArray, QByteArray> &map)
{
	QScriptValue retVal=js->newObject();
	for(auto i=map.begin();i!=map.end();++i)
		retVal.setProperty(QString::fromUtf8(i.key()),QString::fromUtf8(i.value()));
	return retVal;
}