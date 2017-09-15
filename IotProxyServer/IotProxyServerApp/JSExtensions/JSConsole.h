#ifndef JSCONSOLE_H
#define JSCONSOLE_H

#include <QObject>
#include <QScriptEngine>
#include <QFile>
#include <QDebug>

class JSConsole
	:public QObject
{
	Q_OBJECT

public:
	explicit JSConsole(QObject *parent=nullptr);
	Q_INVOKABLE void log(QScriptValue val);

private:
	QString dumpObject(QScriptValue obj,int tabLevel);
	QString dumpArray(QScriptValue arr,int tabLevel);
	QString dumpSimpleValue(QScriptValue val);
};

#endif // JSCONSOLE_H
