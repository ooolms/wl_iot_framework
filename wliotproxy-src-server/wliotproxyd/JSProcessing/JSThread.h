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

#ifndef JSTHREAD_H
#define JSTHREAD_H

#include <QThread>
#include <QScriptEngine>
#include "JSLocalDatabase.h"
#include "../AccessManagement/AccessPolicyTypes.h"

class JSThread
	:public QThread
{
	Q_OBJECT

public:
	explicit JSThread(IdType uid,QObject *parent=0);
	virtual ~JSThread();
	void setFileName(const QString &f);
	void setScriptText(const QString &t);
	void setup();
	QString scriptText();
	QScriptEngine* js();
	void cleanupAfterTerminated();

protected:
	virtual void run();

private slots:
	void onScriptException(const QScriptValue &e);

private:
	QScriptEngine *mJs;
	QString mCode;
	QString mFileName;
	IdType ownerUid;
};

#endif // JSTHREAD_H
