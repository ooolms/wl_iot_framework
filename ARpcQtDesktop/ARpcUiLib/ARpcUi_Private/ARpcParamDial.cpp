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

#include "ARpcParamDial.h"
#include <QDial>
#include <QLayout>
#include <QLabel>

ARpcParamDial::ARpcParamDial(const ARpcControlParam &p,QObject *parent)
	:ARpcIParamElement(parent)
{
	w=new QWidget;
	QLabel *l=new QLabel(p.title,w);
	edit=new QDial(w);

	bool ok=false;
	int v=0;
	if(p.attributes.contains("min"))
		v=p.attributes["min"].toInt(&ok);
	if(ok)edit->setMinimum(v);
	if(p.attributes.contains("max"))
		v=p.attributes["max"].toInt(&ok);
	if(ok)edit->setMaximum(v);
	if(p.attributes.contains("step"))
		v=p.attributes["step"].toInt(&ok);
	if(ok)edit->setSingleStep(v);

	QVBoxLayout *mainLayout=new QVBoxLayout(w);
	mainLayout->setContentsMargins(0,0,0,0);
	mainLayout->addWidget(l);
	mainLayout->addWidget(edit);
	connect(edit,&QDial::sliderReleased,this,&ARpcIParamElement::activated);
}

QByteArray ARpcParamDial::paramValue()
{
	return QByteArray::number(edit->value());
}

QWidget* ARpcParamDial::widget()
{
	return w;
}

void ARpcParamDial::setValue(const QByteArray &v)
{
	bool ok=false;
	int iv=v.toInt(&ok);
	if(!ok)return;
	edit->setValue(iv);
}
