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

#include "ParamDial.h"
#include <QDial>
#include <QLayout>
#include <QLabel>

using namespace WLIOTUi;
using namespace WLIOT;

ParamDial::ParamDial(const WLIOT::ControlParam &p,QObject *parent)
	:IParamElement(parent)
{
	w=new QWidget;
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

	QBoxLayout *lay=new QBoxLayout((p.layout==Qt::Vertical)?QBoxLayout::TopToBottom:QBoxLayout::LeftToRight,w);
	lay->setContentsMargins(0,0,0,0);
	if(!p.title.isEmpty())
		lay->addWidget(new QLabel(p.title,w));
	lay->addWidget(edit);
	connect(edit,&QDial::sliderReleased,this,&IParamElement::activated);
}

QByteArray ParamDial::paramValue()
{
	return QByteArray::number(edit->value());
}

QWidget* ParamDial::widget()
{
	return w;
}

void ParamDial::setValue(const QByteArray &v)
{
	bool ok=false;
	int iv=v.toInt(&ok);
	if(!ok)return;
	edit->setValue(iv);
}
