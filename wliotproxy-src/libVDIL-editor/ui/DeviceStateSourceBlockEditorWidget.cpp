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

#include "DeviceStateSourceBlockEditorWidget.h"
#include "ui_DeviceStateSourceBlockEditorWidget.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

DeviceStateSourceBlockEditorWidget::DeviceStateSourceBlockEditorWidget(IEditorHelper *helper,QWidget *parent)
	:QWidget(parent)
{
	ui=new Ui::DeviceStateSourceBlockEditorWidget;
	ui->setupUi(this);
	mHelper=helper;
	connect(ui->deviceSelectBtn,&QPushButton::clicked,this,&DeviceStateSourceBlockEditorWidget::onSelectDeviceClicked);
	connect(ui->numOutBtn,&QPushButton::toggled,this,&DeviceStateSourceBlockEditorWidget::onNumBtnStateChanged);
}

DeviceStateSourceBlockEditorWidget::~DeviceStateSourceBlockEditorWidget()
{
	delete ui;
}

void DeviceStateSourceBlockEditorWidget::setParams(const QUuid &devId,const QString &devName,
	const QByteArray &stateKey,bool cmdState,bool boolOut,quint32 cmdStateIndex)
{
	ui->deviceIdEdit->setText(devId.toString());
	ui->deviceNameEdit->setText(devName);
	ui->stateKeyEdit->setText(QString::fromUtf8(stateKey));
	if(cmdState)
		ui->cmdStateBtn->setChecked(true);
	else ui->additionalParamBtn->setChecked(true);
	if(boolOut)
		ui->boolOutBtn->setChecked(true);
	else ui->numOutBtn->setChecked(true);
	ui->cmdParamIndexEdit->setValue(cmdStateIndex);
}

QUuid DeviceStateSourceBlockEditorWidget::deviceId() const
{
	return QUuid(ui->deviceIdEdit->text());
}

QByteArray DeviceStateSourceBlockEditorWidget::stateKey() const
{
	return ui->stateKeyEdit->text().toUtf8();
}

bool DeviceStateSourceBlockEditorWidget::commandState() const
{
	return ui->cmdStateBtn->isChecked();
}

bool DeviceStateSourceBlockEditorWidget::boolOut() const
{
	return ui->boolOutBtn->isChecked();
}

quint32 DeviceStateSourceBlockEditorWidget::commandStateIndex()const
{
	return ui->cmdParamIndexEdit->value();
}

void DeviceStateSourceBlockEditorWidget::onSelectDeviceClicked()
{
	QUuid newId;
	QString newName;
	ControlsGroup ctls;
	if(!mHelper->selectDevice(newId,newName,ctls))return;
	if(newId.isNull())return;
	ui->deviceIdEdit->setText(newId.toString());
	ui->deviceNameEdit->setText(newName);
}

void DeviceStateSourceBlockEditorWidget::onNumBtnStateChanged()
{
	ui->cmdParamIndexEdit->setEnabled(ui->cmdStateBtn->isChecked());
}
