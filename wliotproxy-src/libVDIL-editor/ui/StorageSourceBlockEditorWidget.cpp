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
#include "ui_StorageSourceBlockEditorWidget.h"
#include "EditorInternalApi.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

StorageSourceBlockEditorWidget::StorageSourceBlockEditorWidget(IEditorHelper *helper,QWidget *parent)
	:QWidget(parent)
{
	mHelper=helper;
	ui=new Ui::StorageSourceBlockEditorWidget;
	ui->setupUi(this);
	connect(ui->selectStorageBtn,&QPushButton::clicked,this,&StorageSourceBlockEditorWidget::onSelectStorageClicked);
}

StorageSourceBlockEditorWidget::~StorageSourceBlockEditorWidget()
{
	delete ui;
}

void StorageSourceBlockEditorWidget::setParams(
	StorageId stId,const QString &devName,SensorDef::Type valType,quint32 cnt,bool needDevice,bool useTrigger)
{
	mStorId=stId;
	mValuesType=valType;
	ui->deviceIdEdit->setText(stId.deviceId.toString());
	ui->devNameEdit->setText(devName);
	ui->sensorNameEdit->setText(QString::fromUtf8(stId.sensorName));
	ui->valTypeEdit->setText(QString::fromUtf8(valType.toString()));
	ui->needDeviceCheck->setChecked(needDevice);
	ui->useTriggerCheck->setChecked(useTrigger);
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

SensorDef::Type StorageSourceBlockEditorWidget::valuesType()const
{
	return mValuesType;
}

bool StorageSourceBlockEditorWidget::needDevice()const
{
	return ui->needDeviceCheck->isChecked();
}

bool StorageSourceBlockEditorWidget::useTrigger()const
{
	return ui->useTriggerCheck->isChecked();
}

void StorageSourceBlockEditorWidget::onSelectStorageClicked()
{
	StorageId newId;
	QString newDevName;
	SensorDef::Type newType;
	if(!mHelper->selectStorage(newId,newDevName,newType))return;
	if(newId.deviceId.isNull())return;
	mStorId=newId;
	mValuesType=newType;
	ui->deviceIdEdit->setText(mStorId.deviceId.toString());
	ui->devNameEdit->setText(newDevName);
	ui->sensorNameEdit->setText(QString::fromUtf8(mStorId.sensorName));
	ui->valTypeEdit->setText(QString::fromUtf8(mValuesType.toString()));
}
