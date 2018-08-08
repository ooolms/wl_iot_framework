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

#include "JSSensorValue.h"
#include <QScriptValue>
#include <QScriptValueIterator>
#include <QDateTime>

QScriptValue JSSensorValue::sensorValueToJsObject(QScriptEngine *js,const ARpcSensorValue *val)
{
	if(!val)
		return js->nullValue();
	QScriptValue retVal=js->newObject();
	//type
	retVal.setProperty("type",QString::fromUtf8(val->type().toString()),QScriptValue::ReadOnly);
	//time
	if(val->type().tsType==ARpcSensorDef::GLOBAL_TIME)
		retVal.setProperty("time",js->newDate(QDateTime::fromMSecsSinceEpoch(val->time())),QScriptValue::ReadOnly);
	else if(val->type().tsType==ARpcSensorDef::LOCAL_TIME)
		retVal.setProperty("time",(qsreal)val->time(),QScriptValue::ReadOnly);
	//value
	retVal.setProperty("type",js->toScriptValue(QString::fromUtf8(val->type().toString())),QScriptValue::ReadOnly);
	if(val->packetsCount()!=1)
		retVal.setProperty("count",val->packetsCount(),QScriptValue::ReadOnly);
	retVal.setProperty("dim",val->type().dim,QScriptValue::ReadOnly);
	QByteArrayList strs=val->dumpToMsgArgsNoTime();
	QScriptValue vals=js->newArray(strs.count());
	for(int i=0;i<strs.count();++i)
		vals.setProperty(i,js->toScriptValue(QString::fromUtf8(strs[i])),QScriptValue::ReadOnly);
	retVal.setProperty("data",vals);
	return retVal;
}

ARpcSensorValue* JSSensorValue::sensorValueFromJsObject(QScriptEngine *js,const QScriptValue &val)
{
	Q_UNUSED(js)
	if(!val.isObject())
		return 0;
	ARpcSensorDef::Type t;
	t.fromString(val.property("type").toString().toUtf8());
	if(!val.property("data").isArray())return 0;
	QByteArrayList args;
	if(t.tsType!=ARpcSensorDef::NO_TIME)
	{
		QScriptValue timeProp=val.property("time");
		if(timeProp.isDate())
			args.append(QByteArray::number(timeProp.toDateTime().toMSecsSinceEpoch()));
		else if(timeProp.isNumber())
			args.append(QByteArray::number((qint64)timeProp.toNumber()));
		else if(timeProp.isString())
			args.append(timeProp.toString().toUtf8());
		else return 0;
	}
	QScriptValue dataArr=val.property("data");
	quint32 i=0;
	while(true)
	{
		QScriptValue it=dataArr.property(i);
		if(!it.isNumber()&&!it.isString())break;
		if(it.isNumber())
			args.append(QByteArray::number(it.toNumber(),'g',17));
		else args.append(it.toString().toUtf8());
		++i;
	}
	ARpcSensorValue *retVal=ARpcSensorValue::createSensorValue(t);
	if(!retVal->parseMsgArgs(args))
	{
		delete retVal;
		return 0;
	}
	return retVal;
}
