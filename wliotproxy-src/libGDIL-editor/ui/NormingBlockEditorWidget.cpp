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

#include "NormingBlockEditorWidget.h"
#include "ui_NormingBlockEditorUi.h"
#include <QLayout>

NormingBlockEditorWidget::NormingBlockEditorWidget(QWidget *parent)
	:QWidget(parent)
{
	ui=new Ui::NormingBlockEditorWidget;
	ui->setupUi(this);
}

NormingBlockEditorWidget::~NormingBlockEditorWidget()
{
	delete ui;
}

void NormingBlockEditorWidget::setParams(
	const DataUnit &minX,const DataUnit &maxX,const DataUnit &minY,const DataUnit &maxY,
	quint32 dimIndex,bool forceLimits)
{
	ui->minXEdit->setText(minX.value()->valueToString(0));
	ui->maxXEdit->setText(maxX.value()->valueToString(0));
	ui->minYEdit->setText(minY.value()->valueToString(0));
	ui->maxYEdit->setText(maxY.value()->valueToString(0));
	ui->dimEdit->setValue(dimIndex);
	ui->forceLimitsCheck->setChecked(forceLimits);
}

void NormingBlockEditorWidget::limits(DataUnit &minX,DataUnit &maxX,DataUnit &minY,DataUnit &maxY)
{
	bool allOk=true,ok=false;
	qint64 minXS64=ui->minXEdit->text().toLongLong(&ok);
	allOk=allOk&&ok;
	qint64 maxXS64=ui->maxXEdit->text().toLongLong(&ok);
	allOk=allOk&&ok;
	qint64 minYS64=ui->minYEdit->text().toLongLong(&ok);
	allOk=allOk&&ok;
	qint64 maxYS64=ui->maxYEdit->text().toLongLong(&ok);
	allOk=allOk&&ok;
	if(allOk)
	{
		minX=DataUnit(minXS64);
		maxX=DataUnit(maxXS64);
		minY=DataUnit(minYS64);
		maxY=DataUnit(maxYS64);
	}
	else
	{
		allOk=true;
		double minXF64=ui->minXEdit->text().toUtf8().toDouble(&ok);
		allOk=allOk&&ok;
		double maxXF64=ui->maxXEdit->text().toUtf8().toDouble(&ok);
		allOk=allOk&&ok;
		double minYF64=ui->minYEdit->text().toUtf8().toDouble(&ok);
		allOk=allOk&&ok;
		double maxYF64=ui->maxYEdit->text().toUtf8().toDouble(&ok);
		allOk=allOk&&ok;
		if(allOk)
		{
			minX=DataUnit(minXF64);
			maxX=DataUnit(maxXF64);
			minY=DataUnit(minYF64);
			maxY=DataUnit(maxYF64);
		}
	}
}

bool NormingBlockEditorWidget::forceLimits()
{
	return ui->forceLimitsCheck->isChecked();
}

quint32 NormingBlockEditorWidget::dimIndex()
{
	return ui->dimEdit->value();
}
