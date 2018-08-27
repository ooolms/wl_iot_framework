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

#include "ARpcParamTextEdit.h"
#include <QLineEdit>
#include <QLayout>
#include <QLabel>

ARpcParamTextEdit::ARpcParamTextEdit(const ARpcControlParam &p,QObject *parent)
	:ARpcIParamElement(parent)
{
	w=new QWidget;
	QLabel *l=new QLabel(p.title,w);
	edit=new QLineEdit(w);

	QVBoxLayout *mainLayout=new QVBoxLayout(w);
	mainLayout->setContentsMargins(0,0,0,0);
	mainLayout->addWidget(l);
	mainLayout->addWidget(edit);
}

QByteArray ARpcParamTextEdit::paramValue()
{
	return edit->text().toUtf8();
}

QWidget *ARpcParamTextEdit::widget()
{
	return w;
}

void ARpcParamTextEdit::setValue(const QByteArray &v)
{
	edit->setText(v);
}
