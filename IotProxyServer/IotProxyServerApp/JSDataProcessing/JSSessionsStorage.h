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

#ifndef JSSESSIONSSTORAGE_H
#define JSSESSIONSSTORAGE_H

#include "JSISensorStorage.h"

class JSSessionsStorage
	:public JSISensorStorage
{
	Q_OBJECT

public:
	explicit JSSessionsStorage(QScriptEngine *e,ARpcISensorStorage *st,QObject *parent=nullptr);
	Q_INVOKABLE QScriptValue listSessions();
	Q_INVOKABLE bool openSession(QString sessionId);
	Q_INVOKABLE bool closeSession(QString sessionId);
	Q_INVOKABLE QScriptValue getSessionAttribute(QString sessionId,QString key);
	Q_INVOKABLE quint64 valuesCount(QString sessionId);
	Q_INVOKABLE quint64 findInGTIndex(QString sessionId,qint64 ts);
	Q_INVOKABLE QScriptValue valueAt(QString sessionId,quint64 index);
	Q_INVOKABLE bool isSessionOpened(QString sessionId)const;
	Q_INVOKABLE bool setMainReadSessionId(QString sessionId);
};

#endif // JSSESSIONSSTORAGE_H
