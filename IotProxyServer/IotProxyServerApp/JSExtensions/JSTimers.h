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

#ifndef JSTIMERS_H
#define JSTIMERS_H

#include <QObject>
#include <QScriptValue>
#include <QScriptEngine>

class JSTimers
	:public QObject
{
	struct Timer
	{
		QScriptValue cb;
		bool singleShot;
		QScriptValue args;
	};

	Q_OBJECT
public:
	explicit JSTimers(QScriptEngine *e,QObject *parent=0);
	Q_INVOKABLE QScriptValue setInterval(QScriptValue func,QScriptValue msDelay);
	Q_INVOKABLE QScriptValue setTimeout(QScriptValue func,QScriptValue msDelay);
	Q_INVOKABLE QScriptValue setInterval(QScriptValue func,QScriptValue msDelay,QScriptValue args);
	Q_INVOKABLE QScriptValue setTimeout(QScriptValue func,QScriptValue msDelay,QScriptValue args);
	Q_INVOKABLE void clearInterval(QScriptValue id);
	Q_INVOKABLE void clearTimeout(QScriptValue id);

protected:
	virtual void timerEvent(QTimerEvent *e);

private:
	QScriptEngine *js;
	QMap<int,Timer> timers;
};

#endif // JSTIMERS_H
