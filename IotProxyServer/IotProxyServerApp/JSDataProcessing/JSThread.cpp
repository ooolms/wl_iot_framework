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

JSThread::JSThread(const QString &code,QObject *parent)
	:QThread(parent)
{
	mJs=0;
	jsCode=code;
}

JSThread::~JSThread()
{
}

void JSThread::setup()
{
	start();
	while(!isRunning())
		QThread::yieldCurrentThread();
	mJs->moveToThread(this);
}

QScriptEngine* JSThread::js()
{
	return mJs;
}

void JSThread::run()
{
	mJs=new QScriptEngine;
	jsDb=new JSLocalDatabase(mJs,IotProxyInstance::inst().getSensorsDb());
	mJs->globalObject().setProperty("sensorsDatabase",mJs->newQObject(jsDb));
	//TODO load libraries
	mJs->evaluate(jsCode);
	QThread::exec();
	mJs->globalObject().setProperty("sensorsDatabase",mJs->nullValue());
	delete jsDb;
	delete mJs;
}
