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

using namespace WLIOT;

JSISensorStorage::JSISensorStorage(QScriptEngine *e,ISensorStorage *st,QObject *parent)
	:QObject(parent)
{
	stor=st;
	js=e;
	connect(stor,SIGNAL(newValueWritten(const WLIOT::SensorValue*)),
		this,SLOT(onNewValueDirect(const SensorValue*)),Qt::DirectConnection);
}

bool JSISensorStorage::isOpened()
{
	return stor->isOpened();
}

QString JSISensorStorage::valuesType()
{
	return QString::fromUtf8(stor->storedValuesType().toString());
}

QString JSISensorStorage::deviceId()
{
	return stor->deviceId().toString();
}

QString JSISensorStorage::deviceName()
{
	return QString::fromUtf8(stor->deviceName());
}

QString JSISensorStorage::sensorName()
{
	return QString::fromUtf8(stor->sensor().name);
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
	SensorValue *val=stor->valueAt(index);
	if(!val)return js->nullValue();
	return JSSensorValue::sensorValueToJsObject(js,val);
}

QString JSISensorStorage::getStoreMode()
{
	return QString::fromUtf8(stor->storeModeToString(stor->storeMode()));
}

void JSISensorStorage::onNewValueDirect(const SensorValue *value)
{
	SensorValue *v=value->mkCopy();
	QMetaObject::invokeMethod(this,"onNewValueQueued",Qt::QueuedConnection,Q_ARG(void*,(void*)v));
}

void JSISensorStorage::onNewValueQueued(void *value)
{
	SensorValue *v=(SensorValue*)value;
	QScriptValue vv=JSSensorValue::sensorValueToJsObject(js,v);
	emit newValueWritten(vv);
	delete v;
}
