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
#include "../IotProxyInstance.h"
#include "../JSExtensions/JSConsole.h"
#include "JSDevicesList.h"
#include "JSSensorDataTranslator.h"

JSThread::JSThread(const QString &code,QObject *parent)
	:QThread(parent)
{
	mJs=0;
	jsCode=code;
}

JSThread::~JSThread()
{
}

void JSThread::updateScriptText(const QString &t)
{
	if(isRunning())return;
	jsCode=t;
}

void JSThread::setup()
{
	if(isRunning())
		return;
	start();
	while(!mJs)
		QThread::yieldCurrentThread();
	mJs->moveToThread(this);
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

//use Object.getOwnPropertyNames() to see all properties
//"this" in global scope is a global engine object
void JSThread::run()
{
	mJs=new QScriptEngine;
	JSLocalDatabase *jsDb=new JSLocalDatabase(mJs,IotProxyInstance::inst().sensorsStorage(),mJs);
	JSConsole *cons=new JSConsole(mJs);
	JSDevicesList *devs=new JSDevicesList(mJs,mJs);
	mJs->evaluate("script=this;");
	QScriptValue gObj=mJs->globalObject();
	gObj.setProperty("sensorsDatabase",mJs->newQObject(jsDb),QScriptValue::ReadOnly);
	gObj.setProperty("console",mJs->newQObject(cons),QScriptValue::ReadOnly);
	gObj.setProperty("makeDataExportObject",mJs->newFunction(
		&JSSensorDataTranslator::makeTranslator),
		QScriptValue::ReadOnly);
	gObj.setProperty("devices",mJs->newQObject(devs),QScriptValue::ReadOnly);
	//TODO load libraries
	mJs->evaluate(jsCode);
	if(mJs->hasUncaughtException())
	{
		QScriptValue e=mJs->uncaughtException();
		qDebug()<<"Script exception: "<<e.property("lineNumber").toNumber()<<": "<<e.toString();
	}
	QThread::exec();
	gObj.setProperty("sensorsDatabase",mJs->undefinedValue());
	gObj.setProperty("console",mJs->undefinedValue());
	gObj.setProperty("makeDataExportObject",mJs->undefinedValue());
	gObj.setProperty("devices",mJs->undefinedValue());
	QScriptEngine *e=mJs;
	mJs=0;
	delete e;
}
