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

#include "JSISensorStorage.h"
#include "JSSensorValue.h"

JSISensorStorage::JSISensorStorage(QScriptEngine *e,ARpcISensorStorage *st,QObject *parent)
	:QObject(parent)
{
	stor=st;
	js=e;
	connect(stor,&ARpcISensorStorage::newValueWritten,this,&JSISensorStorage::onNewValueDirect,Qt::DirectConnection);
}

bool JSISensorStorage::isOpened()
{
	return stor->isOpened();
}

QString JSISensorStorage::valuesType()
{
	return ARpcSensor::typeToString(stor->effectiveValuesType());
}

QString JSISensorStorage::deviceId()
{
	return stor->deviceId().toString();
}

QString JSISensorStorage::deviceName()
{
	return stor->deviceName();
}

QString JSISensorStorage::sensorName()
{
	return stor->sensor().name;
}

QVariant JSISensorStorage::readAttribute(QString str)
{
	return stor->readAttribute(str.toUtf8());
}

quint64 JSISensorStorage::valuesCount()
{
	return stor->valuesCount();
}

QScriptValue JSISensorStorage::valueAt(quint64 index)
{
	ARpcISensorValue *val=stor->valueAt(index);
	if(!val)return js->nullValue();
	return JSSensorValue::sensorValueToJsObject(js,val);
}

QString JSISensorStorage::getStoreMode()
{
	return stor->storeModeToString(stor->getStoreMode());
}

void JSISensorStorage::onNewValueDirect(const ARpcISensorValue *value)
{
	ARpcISensorValue *v=value->mkCopy();
	QMetaObject::invokeMethod(this,"onNewValueQueued",Qt::QueuedConnection,Q_ARG(void*,(void*)v));
}

void JSISensorStorage::onNewValueQueued(void *value)
{
	ARpcISensorValue *v=(ARpcISensorValue*)value;
	QScriptValue vv=JSSensorValue::sensorValueToJsObject(js,v);
	emit newValueWritten(vv);
	delete v;
}
