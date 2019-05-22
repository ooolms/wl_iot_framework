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

#include "ParamTextEdit.h"
#include <QLineEdit>
#include <QLayout>
#include <QLabel>

ParamTextEdit::ParamTextEdit(const ControlParam &p,QObject *parent)
	:IParamElement(parent)
{
	w=new QWidget;
	edit=new QLineEdit(w);

	QBoxLayout *lay=new QBoxLayout((p.layout==Qt::Vertical)?QBoxLayout::TopToBottom:QBoxLayout::LeftToRight,w);
	lay->setContentsMargins(0,0,0,0);
	if(!p.title.isEmpty())
		lay->addWidget(new QLabel(p.title,w));
	lay->addWidget(edit);

	connect(edit,&QLineEdit::editingFinished,this,&ParamTextEdit::activated);
}

QByteArray ParamTextEdit::paramValue()
{
	return edit->text().toUtf8();
}

QWidget *ParamTextEdit::widget()
{
	return w;
}

void ParamTextEdit::setValue(const QByteArray &v)
{
	edit->setText(v);
}
