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

#include "JSTimers.h"
#include <QTimerEvent>

JSTimers::JSTimers(QScriptEngine *e,QObject *parent)
	:QObject(parent)
{
	js=e;
}

QScriptValue JSTimers::setInterval(QScriptValue func,QScriptValue msDelay)
{
	return setInterval(func,msDelay,js->nullValue());
}

QScriptValue JSTimers::setTimeout(QScriptValue func,QScriptValue msDelay)
{
	return setTimeout(func,msDelay,js->nullValue());
}

QScriptValue JSTimers::setInterval(QScriptValue func,QScriptValue msDelay,QScriptValue args)
{
	if(!func.isFunction()||!msDelay.isNumber())return js->nullValue();
	Timer t={func,false,args};
	int tId=startTimer(msDelay.toUInt32());
	timers[tId]=t;
	return js->toScriptValue(tId);
}

QScriptValue JSTimers::setTimeout(QScriptValue func,QScriptValue msDelay,QScriptValue args)
{
	if(!func.isFunction()||!msDelay.isNumber())return js->nullValue();
	Timer t={func,true,args};
	int tId=startTimer(msDelay.toUInt32());
	timers[tId]=t;
	return js->toScriptValue(tId);
}

void JSTimers::clearInterval(QScriptValue id)
{
	if(!id.isNumber())return;
	int tId=id.toInt32();
	if(timers.contains(tId))
	{
		timers.remove(tId);
		killTimer(tId);
	}
}

void JSTimers::clearTimeout(QScriptValue id)
{
	if(!id.isNumber())return;
	int tId=id.toInt32();
	if(timers.contains(tId))
	{
		timers.remove(tId);
		killTimer(tId);
	}
}

void JSTimers::timerEvent(QTimerEvent *e)
{
	int timerId=e->timerId();
	if(!timers.contains(timerId))
	{
		killTimer(timerId);
		return;
	}
	Timer &t=timers[timerId];
	QScriptValue func=t.cb;
	QScriptValue args=t.args;
	if(t.singleShot)
	{
		killTimer(timerId);
		timers.remove(timerId);
	}
	func.call(js->globalObject(),args);
}
