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

#include "ParamRadio.h"
#include <QLayout>
#include <QRadioButton>
#include <QByteArrayList>
#include <QGroupBox>
#include <QLabel>
#include <QVariant>

using namespace WLIOTUi;
using namespace WLIOT;

ParamRadio::ParamRadio(const ControlParam &p,QObject *parent)
	:IParamElement(parent)
{
	QByteArrayList values,titles;
	if(p.attributes.contains("values"))
		values=p.attributes["values"].split('|');
	if(p.attributes.contains("titles"))
		titles=p.attributes["titles"].split('|');
	for(QByteArray &s:values)
		s=s.trimmed();
	values.removeAll(QByteArray());
	if(values.isEmpty())
		values.append("0");
	w=new QGroupBox;
	QBoxLayout *lay=new QBoxLayout((p.layout==Qt::Vertical)?QBoxLayout::TopToBottom:QBoxLayout::LeftToRight,w);
	if(!p.title.isEmpty())
		lay->addWidget(new QLabel(p.title,w));
	bool useTitles=(titles.count()==values.count());
	for(int i=0;i<values.count();++i)
	{
		QRadioButton *btn=new QRadioButton(QString::fromUtf8(useTitles?titles[i]:values[i]),w);
		btn->setProperty("value",values[i]);
		lay->addWidget(btn);
		btns.append(btn);
		connect(btn,&QRadioButton::clicked,this,&ParamRadio::activated);
	}
	btns[0]->setChecked(true);
}

QByteArray ParamRadio::paramValue()
{
	for(QRadioButton *b:btns)
	{
		if(b->isChecked())
			return b->property("value").toByteArray();
	}
	return QByteArray();
}

QWidget *ParamRadio::widget()
{
	return w;
}

void ParamRadio::setValue(const QByteArray &v)
{
	for(QRadioButton *b:btns)
		b->setChecked(false);
	for(QRadioButton *b:btns)
	{
		if(b->text().toUtf8()==v)
		{
			b->setChecked(true);
			break;
		}
	}
}
