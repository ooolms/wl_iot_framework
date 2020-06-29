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

#include "ArrayCombineBlockEditorWidget.h"
#include <QSpinBox>
#include <QLabel>
#include <QLayout>
#include <limits>

using namespace WLIOTVDIL;

ArrayCombineBlockEditorWidget::ArrayCombineBlockEditorWidget(QWidget *parent)
	:QWidget(parent)
{
	inCountEdit=new QSpinBox(this);
	inCountEdit->setRange(1,std::numeric_limits<int>::max());
	dimEdit=new QSpinBox(this);
	dimEdit->setRange(1,std::numeric_limits<int>::max());
	QLabel *inCountLabel=new QLabel("inputs count");
	QLabel *dimLabel=new QLabel("inputs dim");

	QVBoxLayout *mainLayout=new QVBoxLayout(this);
	mainLayout->addWidget(inCountLabel);
	mainLayout->addWidget(inCountEdit);
	mainLayout->addWidget(dimLabel);
	mainLayout->addWidget(dimEdit);
}

void ArrayCombineBlockEditorWidget::setParams(quint32 inCount,quint32 dim)
{
	inCountEdit->setValue(inCount);
	dimEdit->setValue(dim);
}

quint32 ArrayCombineBlockEditorWidget::inputsCount()const
{
	return inCountEdit->value();
}

quint32 ArrayCombineBlockEditorWidget::dim()const
{
	return dimEdit->value();
}
