#include "JSConsole.h"
#include <QScriptValueIterator>
#include <unistd.h>
#include <QDebug>
#include <QDateTime>

JSConsole::JSConsole(QObject *parent)
	:QObject(parent)
{
}

void JSConsole::log(QScriptValue val)
{
	if(val.isObject())
		qDebug()<<dumpObject(val,0)<<"\n";
	else if(val.isArray())
		qDebug()<<dumpArray(val,0)<<"\n";
	else
		qDebug()<<dumpSimpleValue(val)<<"\n";
}

QString JSConsole::dumpObject(QScriptValue obj,int tabLevel)
{
	QString retVal;
	QDebug d(&retVal);
	d.noquote();
	QString tabs;
	tabs.fill('\t',tabLevel);
	d<<tabs<<"[\n";
	QScriptValueIterator it(obj);
	while(it.hasNext())
	{
		it.next();
		d<<tabs<<"\t"<<it.name()<<" = ";
		QScriptValue v=it.value();
		if(v.isObject())
			d<<dumpObject(v,0);
		else if(v.isArray())
			d<<dumpArray(v,0);
		else
			d<<dumpSimpleValue(v);
		if(it.hasNext())
			d<<",\n";
	}
	d<<tabs<<"\n]";
	return retVal;
}

QString JSConsole::dumpArray(QScriptValue arr,int tabLevel)
{
	QString retVal;
	QDebug d(&retVal);
	d.noquote();
	QString tabs;
	tabs.fill('\t',tabLevel);
	d<<tabs<<"(\n";
	QScriptValueIterator it(arr);
	while(it.hasNext())
	{
		it.next();
		d<<tabs<<"\t"<<it.name()<<" = ";
		QScriptValue v=it.value();
		if(v.isObject())
			d<<dumpObject(v,0);
		else if(v.isArray())
			d<<dumpArray(v,0);
		else
			d<<dumpSimpleValue(v);
		if(it.hasNext())
			d<<",\n";
	}
	d<<tabs<<"\n)";
	return retVal;
}

QString JSConsole::dumpSimpleValue(QScriptValue val)
{
	if(val.isFunction())
		return "[function]";
	else if(val.isBool())
		return val.toBool()?"true":"false";
	else if(val.isDate())
		return val.toDateTime().toString();
	else if(val.isError())
		return "[error]";
	else if(val.isNull())
		return "null";
	else if(val.isNumber())
		return QString::number(val.toNumber());
	else if(val.isQMetaObject())
		return "[Qt MetaObject]";
	else if(val.isQObject())
		return "[Qt Object]";
	else if(val.isRegExp())
		return "QRegExp("+val.toRegExp().pattern()+")";
	else if(val.isString())
		return "\""+val.toString()+"\"";
	else if(val.isUndefined())
		return "undefined";
	else
		return "[other]";
}
