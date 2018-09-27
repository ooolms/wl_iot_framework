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
	QScriptValue vals=js->newArray(val->packetsCount()*val->type().dim);
	if(val->type().numType==ARpcSensorDef::TEXT)
	{
		QByteArrayList strs=val->dumpToMsgArgsNoTime();
		for(int i=0;i<strs.count();++i)
			vals.setProperty(i,js->toScriptValue(QString::fromUtf8(strs[i])),QScriptValue::ReadOnly);
	}
	else
	{
		for(quint32 i=0;i<val->packetsCount();++i)
			for(quint32 j=0;j<val->type().dim;++j)
				vals.setProperty(j+i*val->type().dim,js->toScriptValue(val->valueToDouble(j,i)));
	}
	retVal.setProperty("data",vals);
	return retVal;
}

QByteArrayList JSSensorValue::sensorValueFromJsObject(QScriptEngine *js,const QScriptValue &val)
{
	Q_UNUSED(js)
	if(!val.isObject())
		return QByteArrayList();
	if(!val.property("data").isArray())return QByteArrayList();
	QByteArrayList args;
	QScriptValue timeProp=val.property("time");
	if(timeProp.isDate())
		args.append(QByteArray::number(timeProp.toDateTime().toMSecsSinceEpoch()));
	else if(timeProp.isNumber())
		args.append(QByteArray::number((qint64)timeProp.toNumber()));
	else if(timeProp.isString())
		args.append(timeProp.toString().toUtf8());
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
	return args;
}
