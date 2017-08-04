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

#include "ARpcParamSlider.h"
#include <QSlider>
#include <QLayout>
#include <QLabel>

ARpcParamSlider::ARpcParamSlider(const ARpcControlParam &p,QObject *parent)
	:ARpcIParamElement(parent)
{
	w=new QWidget;
	QLabel *l=new QLabel(p.title,w);
	edit=new QSlider(Qt::Horizontal,w);

	bool ok=false;
	int v;
	if(p.constraints.contains("min"))
		v=p.constraints["min"].toInt(&ok);
	if(ok)edit->setMinimum(v);
	if(p.constraints.contains("max"))
		v=p.constraints["max"].toInt(&ok);
	if(ok)edit->setMaximum(v);
	if(p.constraints.contains("step"))
		v=p.constraints["step"].toInt(&ok);
	if(ok)edit->setSingleStep(v);

	//TODO layout from p
	QVBoxLayout *mainLayout=new QVBoxLayout(w);
	mainLayout->setContentsMargins(0,0,0,0);
	mainLayout->addWidget(l);
	mainLayout->addWidget(edit);
	connect(edit,&QSlider::sliderReleased,this,&ARpcIParamElement::activated);
}

QString ARpcParamSlider::paramValue()
{
	return QString::number(edit->value());
}

QWidget* ARpcParamSlider::widget()
{
	return w;
}

void ARpcParamSlider::setValue(const QString &v)
{
	bool ok=false;
	int iv=v.toInt(&ok);
	if(!ok)return;
	edit->setValue(iv);
}