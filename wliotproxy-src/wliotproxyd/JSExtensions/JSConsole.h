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
