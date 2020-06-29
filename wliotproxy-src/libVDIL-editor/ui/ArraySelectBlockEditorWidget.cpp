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

#include "ArraySelectBlockEditorWidget.h"
#include <QSpinBox>
#include <QLabel>
#include <QLayout>
#include <limits>

using namespace WLIOTVDIL;

ArraySelectBlockEditorWidget::ArraySelectBlockEditorWidget(QWidget *parent)
	:QWidget(parent)
{
	fromEdit=new QSpinBox(this);
	fromEdit->setRange(0,std::numeric_limits<int>::max());
	countEdit=new QSpinBox(this);
	countEdit->setRange(1,std::numeric_limits<int>::max());
	QLabel *fromLabel=new QLabel("start index (0-based)");
	QLabel *countLabel=new QLabel("count");

	QVBoxLayout *mainLayout=new QVBoxLayout(this);
	mainLayout->addWidget(fromLabel);
	mainLayout->addWidget(fromEdit);
	mainLayout->addWidget(countLabel);
	mainLayout->addWidget(countEdit);
}

void ArraySelectBlockEditorWidget::setParams(quint32 startIndex,quint32 endIndex)
{
	fromEdit->setValue(startIndex);
	countEdit->setValue(endIndex);
}

quint32 ArraySelectBlockEditorWidget::from()const
{
	return fromEdit->value();
}

quint32 ArraySelectBlockEditorWidget::count()const
{
	return countEdit->value();
}
