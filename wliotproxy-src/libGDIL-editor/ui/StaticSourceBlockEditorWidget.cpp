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

#include "StaticSourceBlockEditorWidget.h"
#include "StaticSourceBlockEditorWidget.h"
#include <QCheckBox>
#include <QLayout>

StaticSourceBlockEditorWidget::StaticSourceBlockEditorWidget(QWidget *parent)
	:QWidget(parent)
{
	edit=new DataUnitEdit(TypeConstraints(DataUnit::ANY,0),this);
	configurableCheck=new QCheckBox("configurable on-the-fly",this);

	QVBoxLayout *mainLayout=new QVBoxLayout(this);
	mainLayout->addWidget(configurableCheck);
	mainLayout->addWidget(edit);
}

DataUnit StaticSourceBlockEditorWidget::value()const
{
	return edit->value();
}

bool StaticSourceBlockEditorWidget::configurable() const
{
	return configurableCheck->isChecked();
}

void StaticSourceBlockEditorWidget::setParams(const DataUnit &u,bool configurable)
{
	configurableCheck->setChecked(configurable);
	edit->setValue(u);
}
