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

#include "JSSensorValueToObject.h"
#include "ARpcBase/ARpcAllSensorValues.h"
#include <QDateTime>

QScriptValue JSSensorValueToObject::sensorValueToJsObject(QScriptEngine *js,const ARpcISensorValue *val)
{
	if(!val)
		return js->nullValue();
	QScriptValue retVal=js->newObject();
	//type
	retVal.setProperty("type",ARpcSensor::typeToString(val->type()));
	//time
	if(ARpcSensor::isGTValue(val->type()))
		retVal.setProperty("gTime",js->newDate(QDateTime::fromMSecsSinceEpoch(val->time())));
	else if(ARpcSensor::isLTValue(val->type()))
		retVal.setProperty("lTime",(qsreal)val->time());
	//value
	if(ARpcSensor::isSingle(val->type()))
	{
		ARpcSingleSensorValue *sVal=(ARpcSingleSensorValue*)val;
		retVal.setProperty("dims",sVal->dims());
		QScriptValue vals=js->newArray();
		for(int i=0;i<sVal->values().count();++i)
			vals.setProperty(i,sVal->values()[i]);
		retVal.setProperty("data",vals);
	}
	else if(ARpcSensor::isPacket(val->type()))
	{
		ARpcPacketSensorValue *pVal=(ARpcPacketSensorValue*)val;
		retVal.setProperty("dims",pVal->dims());
		retVal.setProperty("count",pVal->valuesCount());
		QScriptValue vals=js->newArray();
		for(int i=0;i<pVal->values().count();++i)
			vals.setProperty(i,pVal->values()[i]);
		retVal.setProperty("data",vals);
	}
	else if(ARpcSensor::isText(val->type()))
	{
		ARpcTextSensorValue *tVal=(ARpcTextSensorValue*)val;
		retVal.setProperty("data",tVal->value());
	}
	return retVal;
}