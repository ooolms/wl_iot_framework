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

#include "JSThread.h"
#include "../ServerInstance.h"
#include "../JSExtensions/JSConsole.h"
#include "../JSExtensions/JSTimers.h"
#include "JSDevicesList.h"

JSThread::JSThread(IdType uid,QObject *parent)
	:QThread(parent)
{
	mJs=0;
	ownerUid=uid;
}

JSThread::~JSThread()
{
}

void JSThread::setFileName(const QString &f)
{
	mFileName=f;
}

void JSThread::setScriptText(const QString &t)
{
	mCode=t;
}

void JSThread::setup()
{
	if(isRunning())
		return;
	start();
	while(!isRunning())
		QThread::yieldCurrentThread();
}

QString JSThread::scriptText()
{
	return mCode;
}

QScriptEngine* JSThread::js()
{
	return mJs;
}

void JSThread::cleanupAfterTerminated()
{
	if(mJs)
		delete mJs;
	mJs=0;
}

//CRIT synchronization ???
//use Object.getOwnPropertyNames() to see all properties
//"this" in global scope is a global engine object
void JSThread::run()
{
	mJs=new QScriptEngine;
	JSLocalDatabase *jsDb=new JSLocalDatabase(mJs,ServerInstance::inst().storages(),ownerUid);
	JSConsole *cons=new JSConsole(mJs);
	JSTimers *timers=new JSTimers(mJs);
	JSDevicesList *devs=new JSDevicesList(mJs,ownerUid);
	connect(mJs,&QScriptEngine::signalHandlerException,this,&JSThread::onScriptException,Qt::DirectConnection);
	mJs->evaluate("script=this;");
	QScriptValue gObj=mJs->globalObject();
	gObj.setProperty("SensorsDatabase",mJs->newQObject(jsDb),QScriptValue::ReadOnly);
	gObj.setProperty("Console",mJs->newQObject(cons),QScriptValue::ReadOnly);
	gObj.setProperty("Timer",mJs->newQObject(timers),QScriptValue::ReadOnly);
	gObj.setProperty("Devices",mJs->newQObject(devs),QScriptValue::ReadOnly);
	//TODO load libraries
	mJs->evaluate(mCode,mFileName);
	if(mJs->hasUncaughtException())
	{
		QScriptValue e=mJs->uncaughtException();
		qDebug()<<"Script exception: "<<mFileName<<":"<<e.property("lineNumber").toNumber()<<": "<<e.toString();
	}
	QThread::exec();
	gObj.setProperty("SensorsDatabase",mJs->undefinedValue());
	gObj.setProperty("Console",mJs->undefinedValue());
	gObj.setProperty("Timer",mJs->undefinedValue());
	gObj.setProperty("Devices",mJs->undefinedValue());
	QScriptEngine *e=mJs;
	mJs=0;
	delete e;
}

void JSThread::onScriptException(const QScriptValue &e)
{
	qDebug()<<"Script exception: "<<e.property("lineNumber").toNumber()<<": "<<e.toString();
}
