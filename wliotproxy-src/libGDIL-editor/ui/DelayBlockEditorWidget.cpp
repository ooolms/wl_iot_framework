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

#include "DelayBlockEditorWidget.h"
#include <QLayout>
#include <QSpinBox>
#include <QLabel>
#include <limits>

DelayBlockEditorWidget::DelayBlockEditorWidget(QWidget *parent)
	:QWidget(parent)
{
	QLabel *l=new QLabel("delay (msec)",this);
	delayEdit=new QSpinBox(this);
	delayEdit->setRange(1,std::numeric_limits<int>::max());
	delayEdit->setValue(1);

	QVBoxLayout *lay=new QVBoxLayout(this);
	lay->addWidget(l);
	lay->addWidget(delayEdit);
}

quint32 DelayBlockEditorWidget::delay()
{
	return delayEdit->value();
}

void DelayBlockEditorWidget::setDelay(quint32 msec)
{
	delayEdit->setValue(msec);
}
