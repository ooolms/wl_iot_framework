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

#include "DebugBlockEditorWidget.h"
#include <QLineEdit>
#include <QLabel>
#include <QSpinBox>
#include <QLayout>

using namespace WLIOTGDIL;

DebugBlockEditorWidget::DebugBlockEditorWidget(QWidget *parent)
	:QWidget(parent)
{
	msgEdit=new QLineEdit(this);
	inCountEdit=new QSpinBox(this);
	inCountEdit->setRange(1,999);
	QLabel *msgLabel=new QLabel("debug message",this);
	QLabel *cntLabel=new QLabel("inputs count",this);

	QVBoxLayout *mainLayout=new QVBoxLayout(this);
	mainLayout->addWidget(msgLabel);
	mainLayout->addWidget(msgEdit);
	mainLayout->addWidget(cntLabel);
	mainLayout->addWidget(inCountEdit);
}

void DebugBlockEditorWidget::setParams(const QString &debugString,quint32 inCount)
{
	msgEdit->setText(debugString);
	inCountEdit->setValue(inCount);
}

QString DebugBlockEditorWidget::debugString()const
{
	return msgEdit->text();
}

quint32 DebugBlockEditorWidget::inCount()const
{
	return inCountEdit->value();
}
