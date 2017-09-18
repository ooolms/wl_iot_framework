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

#ifndef JSSENSORDATATRANSLATOR_H
#define JSSENSORDATATRANSLATOR_H

#include <QObject>
#include "../ISensorDataTranslator.h"
#include "QScriptEngine"

class JSSensorDataTranslator
	:public QObject
{
	Q_OBJECT

public:
	explicit JSSensorDataTranslator(ISensorDataTranslator *t,QScriptEngine *e,QObject *parent=nullptr);
	static QScriptValue makeTranslator(QScriptContext *ctx,QScriptEngine *js);
	Q_INVOKABLE void writeSensorValue(QScriptValue val);
	Q_INVOKABLE bool checkConfig(QScriptValue cfg);

private:
	QScriptEngine *js;
	ISensorDataTranslator *transl;
};

#endif // JSSENSORDATATRANSLATOR_H
