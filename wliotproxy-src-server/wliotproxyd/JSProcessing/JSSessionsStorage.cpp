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

#include "JSSessionsStorage.h"
#include "wliot/storages/SessionStorage.h"
#include "JSSensorValue.h"

using namespace WLIOT;

JSSessionsStorage::JSSessionsStorage(QScriptEngine *e,ISensorStorage *st,QObject *parent)
	:JSISensorStorage(e,st,parent)
{
}

QScriptValue JSSessionsStorage::listSessions()
{
	QList<QUuid> ids;
	QByteArrayList titles;
	if(!reinterpret_cast<SessionStorage*>(stor)->listSessions(ids,titles))
		return js->nullValue();
	QScriptValue arr=js->newArray();
	for(int i=0;i<ids.count();++i)
	{
		QScriptValue o=js->newObject();
		o.setProperty("id",ids[i].toString(),QScriptValue::ReadOnly);
		o.setProperty("title",QString::fromUtf8(titles[i]),QScriptValue::ReadOnly);
		arr.setProperty(i,o,QScriptValue::ReadOnly);
	}
	return arr;
}

bool JSSessionsStorage::openSession(QScriptValue sessionId)
{
	QUuid id(sessionId.toString());
	if(id.isNull())return false;
	return (reinterpret_cast<SessionStorage*>(stor))->openSession(id);
}

bool JSSessionsStorage::closeSession(QScriptValue sessionId)
{
	QUuid id(sessionId.toString());
	if(id.isNull())return false;
	return (reinterpret_cast<SessionStorage*>(stor))->closeSession(id);
}

QScriptValue JSSessionsStorage::getSessionAttribute(QScriptValue sessionId,QScriptValue key)
{
	QUuid id(sessionId.toString());
	if(id.isNull())return js->nullValue();
	QByteArray var;
	if(!reinterpret_cast<SessionStorage*>(stor)->getSessionAttribute(
		id.toByteArray(),key.toString().toUtf8(),var))
		return js->nullValue();
	return js->toScriptValue(var);
}

quint64 JSSessionsStorage::valuesCount(QScriptValue sessionId)
{
	QUuid id(sessionId.toString());
	if(id.isNull())return 0;
	return reinterpret_cast<SessionStorage*>(stor)->valuesCount(id);
}

quint64 JSSessionsStorage::findInGTIndex(QScriptValue sessionId,QScriptValue ts)
{
	if(!ts.isNumber())
		return 0;
	QUuid id(sessionId.toString());
	if(id.isNull())
		return 0;
	return reinterpret_cast<SessionStorage*>(stor)->findInGTIndex(id,(quint64)ts.toNumber());
}

QScriptValue JSSessionsStorage::valueAt(QScriptValue sessionId,QScriptValue index)
{
	if(!index.isNumber())
		return js->nullValue();
	QUuid id(sessionId.toString());
	if(id.isNull())
		return js->nullValue();
	SensorValue *val=reinterpret_cast<SessionStorage*>(stor)->valueAt(id,(quint64)index.toNumber());
	if(!val)return js->nullValue();
	QScriptValue jsVal=JSSensorValue::sensorValueToJsObject(js,val);
	delete val;
	return jsVal;
}

bool JSSessionsStorage::isSessionOpened(QScriptValue sessionId)const
{
	QUuid id(sessionId.toString());
	if(id.isNull())return false;
	return reinterpret_cast<SessionStorage*>(stor)->isSessionOpened(id);
}

bool JSSessionsStorage::setMainReadSessionId(QScriptValue sessionId)
{
	QUuid id(sessionId.toString());
	if(id.isNull())return false;
	return reinterpret_cast<SessionStorage*>(stor)->setMainReadSessionId(id);
}
