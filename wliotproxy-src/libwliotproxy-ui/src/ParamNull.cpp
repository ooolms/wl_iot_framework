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

#include "ParamNull.h"

using namespace WLIOTUi;
using namespace WLIOT;

ParamNull::ParamNull(QObject *parent)
	:IParamElement(parent)
{
	value="null";
}

QByteArray ParamNull::paramValue()
{
	return value;
}

QWidget* ParamNull::widget()
{
	return 0;
}

void ParamNull::setValue(const QByteArray &v)
{
	value=v;
}
