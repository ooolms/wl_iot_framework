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

#include "JSSensorDataTranslator.h"
#include "JSSensorValue.h"
#include <QScriptValueIterator>

JSSensorDataTranslator::JSSensorDataTranslator(ISensorDataTranslator *t,QScriptEngine *e,QObject *parent)
	:QObject(parent)
{
	transl=t;
	js=e;
}

QScriptValue JSSensorDataTranslator::makeTranslator(QScriptContext *ctx,QScriptEngine *js)
{
	if(ctx->argumentCount()!=2)
		return js->nullValue();
	QScriptValue typeVal=ctx->argument(0);
	QScriptValue cfgVal=ctx->argument(1);
	if(!typeVal.isString()||!cfgVal.isObject())
		return js->nullValue();
	QVariantMap cfgMap=cfgVal.toVariant().toMap();
	ISensorDataTranslator *transl=ISensorDataTranslator::makeTranslator(typeVal.toString(),cfgMap);
	if(!transl)
		return js->nullValue();
	JSSensorDataTranslator *jsTransl=new JSSensorDataTranslator(transl,js);
	return js->newQObject(jsTransl,QScriptEngine::ScriptOwnership);
}

void JSSensorDataTranslator::writeSensorValue(QScriptValue val)
{
	QScopedPointer<ARpcISensorValue> v(JSSensorValue::sensorValueFromJsObject(js,val));
	if(v.isNull())return;
	transl->writeSensorValue(v.data());
}

bool JSSensorDataTranslator::checkConfig(QScriptValue cfg)
{
	if(!cfg.isObject())
		return false;
	QVariantMap map=cfg.toVariant().toMap();
	return transl->checkConfig(map);
}
