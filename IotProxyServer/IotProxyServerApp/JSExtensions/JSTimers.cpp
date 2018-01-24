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
