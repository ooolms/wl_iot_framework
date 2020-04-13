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

#include "StorageSourceBlockEditorWidget.h"
#include "ui_StorageSourceBlockEditorUi.h"
#include "EditorInternalApi.h"

StorageSourceBlockEditorWidget::StorageSourceBlockEditorWidget(EditorInternalApi *edApi,QWidget *parent)
	:QWidget(parent)
{
	editor=edApi;
	ui=new Ui::StorageSourceBlockEditorWidget;
	ui->setupUi(this);
	connect(ui->selectStorageBtn,&QPushButton::clicked,this,&StorageSourceBlockEditorWidget::onSelectStorageClicked);
}

StorageSourceBlockEditorWidget::~StorageSourceBlockEditorWidget()
{
	delete ui;
}

void StorageSourceBlockEditorWidget::setParams(
	StorageId stId,const QString &devName,SensorDef::Type valType,quint32 cnt)
{
	mStorId=stId;
	mValuesType=valType;
	ui->deviceIdEdit->setText(stId.deviceId.toString());
	ui->devNameEdit->setText(devName);
	ui->sensorNameEdit->setText(QString::fromUtf8(stId.sensorName));
	ui->valTypeEdit->setText(QString::fromUtf8(valType.toString()));
	if(valType.packType==SensorDef::PACKET)
	{
		ui->valuesCountEdit->setEnabled(false);
		ui->valuesCountEdit->setValue(1);
	}
	else
	{
		ui->valuesCountEdit->setEnabled(true);
		ui->valuesCountEdit->setValue(cnt);
	}
}

quint32 StorageSourceBlockEditorWidget::count()const
{
	return ui->valuesCountEdit->value();
}

StorageId StorageSourceBlockEditorWidget::storageId()const
{
	return mStorId;
}

QString StorageSourceBlockEditorWidget::devName()const
{
	return ui->devNameEdit->text();
}

SensorDef::Type StorageSourceBlockEditorWidget::valuesType()const
{
	return mValuesType;
}

void StorageSourceBlockEditorWidget::onSelectStorageClicked()
{
	StorageId newId;
	QString newDevName;
	SensorDef::Type newType;
	editor->selectStorage(newId,newDevName,newType);
	if(newId.deviceId.isNull())return;
	mStorId=newId;
	mValuesType=newType;
	ui->deviceIdEdit->setText(mStorId.deviceId.toString());
	ui->devNameEdit->setText(newDevName);
	ui->sensorNameEdit->setText(QString::fromUtf8(mStorId.sensorName));
	ui->valTypeEdit->setText(QString::fromUtf8(mValuesType.toString()));
}
