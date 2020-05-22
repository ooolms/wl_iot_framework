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

#include "ComparationBlockEditorWidget.h"
#include "ui_ComparationBlockEditorWidget.h"

using namespace WLIOT;
using namespace WLIOTGDIL;

ComparationBlockEditorWidget::ComparationBlockEditorWidget(QWidget *parent)
	:QWidget(parent)
{
	ui=new Ui::ComparationBlockEditorWidget;
	ui->setupUi(this);

	connect(ui->distBtn,&QRadioButton::toggled,this,&ComparationBlockEditorWidget::onDistCheckToggled);
	connect(ui->internalV2Btn,&QRadioButton::toggled,this,&ComparationBlockEditorWidget::onInternalV2CheckToggled);
}

ComparationBlockEditorWidget::~ComparationBlockEditorWidget()
{
	delete ui;
}

void ComparationBlockEditorWidget::setParams(ComparationBlock::OutMode outMode,
	bool extV2Input,quint32 dimIndex,ComparationBlock::Operation op)
{
	if(outMode==ComparationBlock::SPLITTED_BOOL)
		ui->splittedBoolOutputBtn->setChecked(true);
	else if(outMode==ComparationBlock::SPLITTED_INPUT)
		ui->splittedV1OutputBtn->setChecked(true);
	else ui->boolOutputBtn->setChecked(true);
	ui->dimEdit->setValue(dimIndex);
	if(op==ComparationBlock::NEQ)
		ui->neqBtn->setChecked(true);
	else if(op==ComparationBlock::GT)
		ui->gtBtn->setChecked(true);
	else if(op==ComparationBlock::LT)
		ui->ltBtn->setChecked(true);
	else if(op==ComparationBlock::GTEQ)
		ui->gtEqBtn->setChecked(true);
	else if(op==ComparationBlock::LTEQ)
		ui->ltEqBtn->setChecked(true);
	else if(op==ComparationBlock::DISTANCE)
		ui->distBtn->setChecked(true);
	else ui->eqBtn->setChecked(true);
	ui->externalV2Btn->setChecked(extV2Input);
	ui->internalV2Btn->setChecked(!extV2Input);
}

void ComparationBlockEditorWidget::setDistValue(const DataUnit &v)
{
	ui->distEdit->setText(QString::fromUtf8(v.value()->valueToString(0)));
}

void ComparationBlockEditorWidget::setV2Value(const DataUnit &v)
{
	ui->v2ValueEdit->setText(QString::fromUtf8(v.value()->valueToString(0)));
}

ComparationBlock::OutMode ComparationBlockEditorWidget::outMode()const
{
	if(ui->splittedBoolOutputBtn->isChecked())
		return ComparationBlock::SPLITTED_BOOL;
	else if(ui->splittedV1OutputBtn->isChecked())
		return ComparationBlock::SPLITTED_INPUT;
	else return ComparationBlock::SINGLE_BOOL;
}

DataUnit ComparationBlockEditorWidget::distValue()const
{
	return DataUnit::single1DimValueFromString(ui->distEdit->text());
}

quint32 ComparationBlockEditorWidget::dimIndex()const
{
	return ui->dimEdit->value();
}

ComparationBlock::Operation ComparationBlockEditorWidget::operation()const
{
	ComparationBlock::Operation op;
	if(ui->neqBtn->isChecked())
		op=ComparationBlock::NEQ;
	else if(ui->ltBtn->isChecked())
		op=ComparationBlock::LT;
	else if(ui->gtBtn->isChecked())
		op=ComparationBlock::GT;
	else if(ui->ltEqBtn->isChecked())
		op=ComparationBlock::LTEQ;
	else if(ui->gtEqBtn->isChecked())
		op=ComparationBlock::GTEQ;
	else if(ui->distBtn->isChecked())
		op=ComparationBlock::DISTANCE;
	else op=ComparationBlock::EQ;
	return op;
}

bool ComparationBlockEditorWidget::externalV2Input()const
{
	return ui->externalV2Btn->isChecked();
}

DataUnit ComparationBlockEditorWidget::v2Value()const
{
	return DataUnit::single1DimValueFromString(ui->v2ValueEdit->text());
}

void ComparationBlockEditorWidget::onDistCheckToggled()
{
	ui->distEdit->setEnabled(ui->distBtn->isChecked());
}

void ComparationBlockEditorWidget::onInternalV2CheckToggled()
{
	ui->v2ValueEdit->setEnabled(ui->internalV2Btn->isChecked());
}
