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

#include "JSContinuousStorage.h"
#include "wliot/storages/ContinuousStorage.h"

using namespace WLIOT;

JSContinuousStorage::JSContinuousStorage(QScriptEngine *e,ISensorStorage *st,QObject *parent)
	:JSISensorStorage(e,st,parent)
{
}

quint64 JSContinuousStorage::findInGTIndex(qint64 ts)
{
	return reinterpret_cast<ContinuousStorage*>(stor)->findInGTIndex(ts);
}
