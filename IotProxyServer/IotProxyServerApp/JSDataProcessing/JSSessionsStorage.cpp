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
#include "ARpcLocalStorage/ARpcSessionStorage.h"
#include "JSSensorValueToObject.h"

JSSessionsStorage::JSSessionsStorage(QScriptEngine *e,ARpcISensorStorage *st,QObject *parent)
	:JSISensorStorage(e,st,parent)
{
}

QScriptValue JSSessionsStorage::listSessions()
{
	QList<QUuid> ids;
	QStringList titles;
	if(!((ARpcSessionStorage*)stor)->listSessions(ids,titles))
		return js->nullValue();
	QScriptValue arr=js->newArray();
	for(int i=0;i<ids.count();++i)
	{
		QScriptValue o=js->newObject();
		o.setProperty("id",ids[i].toString(),QScriptValue::ReadOnly);
		o.setProperty("title",titles[i],QScriptValue::ReadOnly);
		arr.setProperty(i,o,QScriptValue::ReadOnly);
	}
	return arr;
}

bool JSSessionsStorage::openSession(QString sessionId)
{
	QUuid id(sessionId);
	if(id.isNull())return false;
	return ((ARpcSessionStorage*)stor)->openSession(id);
}

bool JSSessionsStorage::closeSession(QString sessionId)
{
	QUuid id(sessionId);
	if(id.isNull())return false;
	return ((ARpcSessionStorage*)stor)->closeSession(id);
}

QScriptValue JSSessionsStorage::getSessionAttribute(QString sessionId,QString key)
{
	QVariant var;
	if(!((ARpcSessionStorage*)stor)->getSessionAttribute(sessionId,key,var))
		return js->nullValue();
	return js->newVariant(var);
}

quint64 JSSessionsStorage::valuesCount(QString sessionId)
{
	QUuid id(sessionId);
	if(id.isNull())return 0;
	return ((ARpcSessionStorage*)stor)->valuesCount(id);
}

quint64 JSSessionsStorage::findInGTIndex(QString sessionId, qint64 ts)
{
	QUuid id(sessionId);
	if(id.isNull())return false;
	return ((ARpcSessionStorage*)stor)->findInGTIndex(id,ts);
}

QScriptValue JSSessionsStorage::valueAt(QString sessionId, quint64 index)
{
	QUuid id(sessionId);
	if(id.isNull())return false;
	ARpcISensorValue *val=((ARpcSessionStorage*)stor)->valueAt(id,index);
	if(!val)return js->nullValue();
	return JSSensorValueToObject::sensorValueToJsObject(js,val);
}

bool JSSessionsStorage::isSessionOpened(QString sessionId)const
{
	QUuid id(sessionId);
	if(id.isNull())return false;
	return ((ARpcSessionStorage*)stor)->isSessionOpened(id);
}

bool JSSessionsStorage::setMainReadSessionId(QString sessionId)
{
	QUuid id(sessionId);
	if(id.isNull())return false;
	return ((ARpcSessionStorage*)stor)->setMainReadSessionId(id);
}
