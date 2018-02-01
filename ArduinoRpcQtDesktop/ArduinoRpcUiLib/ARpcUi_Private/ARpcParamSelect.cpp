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

#include "ARpcParamSelect.h"
#include <QComboBox>
#include <QLayout>
#include <QLabel>

ARpcParamSelect::ARpcParamSelect(const ARpcControlParam &p,QObject *parent)
	:ARpcIParamElement(parent)
{
	w=new QWidget;
	QLabel *l=new QLabel(p.title,w);
	edit=new QComboBox(w);

	QByteArrayList values;
	QStringList valuesUnic;
	if(p.constraints.contains("values"))
		values=p.constraints["values"].split(';');
	for(QByteArray &s:values)
		valuesUnic.append(QString::fromUtf8(s.trimmed()));
	valuesUnic.removeAll(QString());
	if(valuesUnic.isEmpty())valuesUnic.append("0");

	edit->addItems(valuesUnic);

	//TODO layout from p
	QHBoxLayout *mainLayout=new QHBoxLayout(w);
	mainLayout->setContentsMargins(0,0,0,0);
	mainLayout->addWidget(l);
	mainLayout->addWidget(edit);
	connect(edit,static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),this,&ARpcIParamElement::activated);

}

QByteArray ARpcParamSelect::paramValue()
{
	return edit->currentText().toUtf8();
}

QWidget* ARpcParamSelect::widget()
{
	return w;
}

void ARpcParamSelect::setValue(const QByteArray &v)
{
	edit->setCurrentText(QString::fromUtf8(v));
}
