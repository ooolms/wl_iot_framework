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

#include "OneStringVarSelectWidget.h"
#include <QComboBox>
#include <QLayout>
#include <QLabel>

using namespace WLIOTVDIL;

OneStringVarSelectWidget::OneStringVarSelectWidget(const QString &labelText,QWidget *parent)
	:QWidget(parent)
{
	mSelect=new QComboBox(this);
	QLabel *l=new QLabel(labelText,this);

	QHBoxLayout *mainLayout=new QHBoxLayout(this);
	mainLayout->addWidget(l);
	mainLayout->addWidget(mSelect);
}

void OneStringVarSelectWidget::setVariants(const QStringList &allVariants,const QString &currentVariant)
{
	mSelect->clear();
	mSelect->addItems(allVariants);
	if(allVariants.contains(currentVariant))
		mSelect->setCurrentText(currentVariant);
}

QString OneStringVarSelectWidget::selectedVariant()
{
	return mSelect->currentText();
}
