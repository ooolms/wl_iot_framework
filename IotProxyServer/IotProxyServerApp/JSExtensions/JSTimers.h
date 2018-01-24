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
