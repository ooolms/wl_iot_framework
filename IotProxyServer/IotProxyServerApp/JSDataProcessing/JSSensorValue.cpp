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
#include "ARpcBase/ARpcAllSensorValues.h"
#include <QScriptValue>
#include <QScriptValueIterator>
#include <QDateTime>

QScriptValue JSSensorValue::sensorValueToJsObject(QScriptEngine *js,const ARpcISensorValue *val)
{
	if(!val)
		return js->nullValue();
	QScriptValue retVal=js->newObject();
	//type
	retVal.setProperty("type",QString::fromUtf8(ARpcSensor::typeToString(val->type())),QScriptValue::ReadOnly);
	//time
	if(ARpcSensor::isGTValue(val->type()))
		retVal.setProperty("gTime",js->newDate(QDateTime::fromMSecsSinceEpoch(val->time())),QScriptValue::ReadOnly);
	else if(ARpcSensor::isLTValue(val->type()))
		retVal.setProperty("lTime",(qsreal)val->time(),QScriptValue::ReadOnly);
	//value
	if(ARpcSensor::isSingle(val->type()))
	{
		ARpcSingleSensorValue *sVal=(ARpcSingleSensorValue*)val;
		retVal.setProperty("dims",sVal->dims(),QScriptValue::ReadOnly);
		QScriptValue vals=js->newArray(sVal->values().count());
		for(int i=0;i<sVal->values().count();++i)
			vals.setProperty(i,(qsreal)sVal->values()[i],QScriptValue::ReadOnly);
		retVal.setProperty("data",vals,QScriptValue::ReadOnly);
	}
	else if(ARpcSensor::isPacket(val->type()))
	{
		ARpcPacketSensorValue *pVal=(ARpcPacketSensorValue*)val;
		retVal.setProperty("dims",pVal->dims(),QScriptValue::ReadOnly);
		retVal.setProperty("count",pVal->valuesCount(),QScriptValue::ReadOnly);
		QScriptValue vals=js->newArray(pVal->values().count());
		for(int i=0;i<pVal->values().count();++i)
			vals.setProperty(i,(qsreal)pVal->values()[i],QScriptValue::ReadOnly);
		retVal.setProperty("data",vals,QScriptValue::ReadOnly);
	}
	else if(ARpcSensor::isText(val->type()))
	{
		ARpcTextSensorValue *tVal=(ARpcTextSensorValue*)val;
		retVal.setProperty("data",QString::fromUtf8(tVal->value()),QScriptValue::ReadOnly);
	}
	return retVal;
}

ARpcISensorValue* JSSensorValue::sensorValueFromJsObject(QScriptEngine *js,const QScriptValue &val)
{
	Q_UNUSED(js)
	if(!val.isObject())
		return 0;
	ARpcSensor::Type t=ARpcSensor::typeFromString(val.property("type").toString().toUtf8());
	if(ARpcSensor::isSingle(t))
	{
		if(!val.property("dims").isNumber())return 0;
		if(!val.property("data").isArray())return 0;
		quint32 dims=val.property("dims").toUInt32();
		QVector<ARpcSingleSensorValue::ValueType> vals;
		QScriptValue dataArr=val.property("data");
		quint32 i=0;
		while(dataArr.property(i).isNumber())
		{
			vals.append(dataArr.property(i).toNumber());
			++i;
		}
		if((quint32)vals.count()!=dims)return 0;
		ARpcSingleSensorValue *retVal;
		if(t==ARpcSensor::SINGLE)
			retVal=new ARpcSingleSensorValue(dims);
		else if(t==ARpcSensor::SINGLE_GT)
		{
			retVal=new ARpcSingleSensorValue(dims,false);
			retVal->setTime((quint64)val.property("gTime").toNumber());
		}
		else if(t==ARpcSensor::SINGLE_LT)
		{
			retVal=new ARpcSingleSensorValue(dims,true);
			retVal->setTime((quint64)val.property("lTime").toNumber());
		}
		else return 0;
		retVal->fromData(vals);
		return retVal;
	}
	else if(ARpcSensor::isPacket(t))
	{
		if(!val.property("dims").isNumber())return 0;
		if(!val.property("count").isNumber())return 0;
		if(!val.property("data").isArray())return 0;
		quint32 dims=val.property("dims").toUInt32();
		quint32 count=val.property("count").toUInt32();
		QVector<ARpcPacketSensorValue::ValueType> vals;
		QScriptValue dataArr=val.property("data");
		quint32 i=0;
		while(dataArr.property(i).isNumber())
		{
			vals.append(dataArr.property(i).toNumber());
			++i;
		}
		if((quint32)vals.count()!=(dims*count))return 0;
		ARpcPacketSensorValue *retVal;
		if(t==ARpcSensor::PACKET)
			retVal=new ARpcPacketSensorValue(dims);
		else if(t==ARpcSensor::PACKET_GT)
		{
			retVal=new ARpcPacketSensorValue(dims,false);
			retVal->setTime((quint64)val.property("gTime").toNumber());
		}
		else if(t==ARpcSensor::PACKET_LT)
		{
			retVal=new ARpcPacketSensorValue(dims,true);
			retVal->setTime((quint64)val.property("lTime").toNumber());
		}
		else return 0;
		retVal->fromData(vals,dims);
		return retVal;
	}
	else if(ARpcSensor::isText(t))
	{
		if(!val.property("data").isString())return 0;
		ARpcTextSensorValue *retVal=new ARpcTextSensorValue;
		retVal->setTime((quint64)val.property("gTime").toNumber());
		retVal->fromData(val.property("data").toString().toUtf8());
		return retVal;
	}
	return 0;
}
