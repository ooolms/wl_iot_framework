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

#include "ParamCheckbox.h"
#include <QCheckBox>

using namespace WLIOTUi;
using namespace WLIOT;

ParamCheckbox::ParamCheckbox(const ControlParam &p,QObject *parent)
	:IParamElement(parent)
{
	onValue="1";
	offValue="0";
	if(p.attributes.contains("onValue")&&!p.attributes["onValue"].isEmpty())
		onValue=p.attributes["onValue"];
	if(p.attributes.contains("offValue")&&!p.attributes["offValue"].isEmpty())
		offValue=p.attributes["offValue"];
	check=new QCheckBox(p.title);
	connect(check,&QCheckBox::clicked,this,&IParamElement::activated);
}

QByteArray ParamCheckbox::paramValue()
{
	if(check->isChecked())return onValue;
	else return offValue;
}

QWidget* ParamCheckbox::widget()
{
	return check;
}

void ParamCheckbox::setValue(const QByteArray &v)
{
	if(v==offValue)check->setChecked(false);
	else if(v==onValue)check->setChecked(true);
}
