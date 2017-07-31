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

#include "ISensorDataTranslator.h"
#include "IotkitAgentSensorDataTranslator.h"

ISensorDataTranslator::ISensorDataTranslator(const QVariantMap &cfg,QObject *parent)
	:QObject(parent)
{
	config=cfg;
}

ISensorDataTranslator* ISensorDataTranslator::makeTranslator(const QString &type,const QVariantMap &cfg)
{
	if(type==IotkitAgentSensorDataTranslator::type)
		return new IotkitAgentSensorDataTranslator(cfg);
	return 0;
}
