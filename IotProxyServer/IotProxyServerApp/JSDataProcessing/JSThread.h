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

class JSThread
	:public QThread
{
	Q_OBJECT

public:
	explicit JSThread(const QString &code,QObject *parent=0);
	virtual ~JSThread();
	void updateScriptText(const QString &t);
	void setup();
	QScriptEngine* js();
	void cleanupAfterTerminated();

protected:
	virtual void run();

private:
	QScriptEngine *mJs;
	QString jsCode;
};

#endif // JSTHREAD_H
