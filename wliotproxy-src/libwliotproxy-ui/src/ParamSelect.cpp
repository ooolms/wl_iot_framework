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

#include "ParamSelect.h"
#include <QComboBox>
#include <QLayout>
#include <QLabel>

using namespace WLIOTUi;
using namespace WLIOT;

ParamSelect::ParamSelect(const ControlParam &p,QObject *parent)
	:IParamElement(parent)
{
	w=new QWidget;
	edit=new QComboBox(w);

	QByteArrayList values,titles;
	if(p.attributes.contains("values"))
		values=p.attributes["values"].split('|');
	if(p.attributes.contains("titles"))
		titles=p.attributes["titles"].split('|');

	bool useTitles=(titles.count()==values.count());
	for(int i=0;i<values.count();++i)
		edit->addItem(QString::fromUtf8(useTitles?titles[i]:values[i]),values[i]);

	QBoxLayout *lay=new QBoxLayout((p.layout==Qt::Vertical)?QBoxLayout::TopToBottom:QBoxLayout::LeftToRight,w);
	lay->setContentsMargins(0,0,0,0);
	if(!p.title.isEmpty())
		lay->addWidget(new QLabel(p.title,w));
	lay->addWidget(edit);
	connect(edit,static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),this,&IParamElement::activated);

}

QByteArray ParamSelect::paramValue()
{
	QByteArray value=edit->currentData().toByteArray();
	if(value.isEmpty())
		return edit->currentText().toUtf8();
	else return value;
}

QWidget* ParamSelect::widget()
{
	return w;
}

void ParamSelect::setValue(const QByteArray &v)
{
	edit->setCurrentText(QString::fromUtf8(v));
}
