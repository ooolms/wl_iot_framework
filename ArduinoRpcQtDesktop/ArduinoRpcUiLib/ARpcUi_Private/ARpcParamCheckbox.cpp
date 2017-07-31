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

#include "ARpcParamCheckbox.h"
#include <QCheckBox>

ARpcParamCheckbox::ARpcParamCheckbox(const ARpcControlParam &p,QObject *parent)
	:ARpcIParamElement(parent)
{
	onValue="1";
	offValue="0";
	if(p.constraints.contains("onValue")&&!p.constraints["onValue"].isEmpty())
		onValue=p.constraints["onValue"];
	if(p.constraints.contains("offValue")&&!p.constraints["offValue"].isEmpty())
		offValue=p.constraints["offValue"];
	check=new QCheckBox(p.title);
	connect(check,&QCheckBox::toggled,this,&ARpcIParamElement::activated);
}

QString ARpcParamCheckbox::paramValue()
{
	if(check->isChecked())return onValue;
	else return offValue;
}

QWidget* ARpcParamCheckbox::widget()
{
	return check;
}

void ARpcParamCheckbox::setValue(const QString &v)
{
	if(v==offValue)check->setChecked(false);
	else if(v==onValue)check->setChecked(true);
}
