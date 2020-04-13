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

#include "CommandBlockEditorWidget.h"
#include "ui_CommandBlockEditorUi.h"
#include "EditorInternalApi.h"
#include <QUuid>

CommandBlockEditorWidget::CommandBlockEditorWidget(EditorInternalApi *edApi,QWidget *parent)
	:QWidget(parent)
{
	editor=edApi;
	ui=new Ui::CommandBlockEditorWidget;
	ui->setupUi(this);
	ui->argsList->setDragDropMode(QAbstractItemView::InternalMove);
	connect(ui->addArgBtn,&QPushButton::clicked,this,&CommandBlockEditorWidget::onAddArgClicked);
	connect(ui->delArgBtn,&QPushButton::clicked,this,&CommandBlockEditorWidget::onDelArgClicked);
	connect(ui->deviceSelectBtn,&QPushButton::clicked,this,&CommandBlockEditorWidget::onSelectDevClicked);
}

CommandBlockEditorWidget::~CommandBlockEditorWidget()
{
	delete ui;
}

void CommandBlockEditorWidget::setParams(
	const QUuid &devId,const QByteArray &devName,const QByteArray &cmd,const QByteArrayList &args,quint32 inCount)
{
	ui->deviceIdEdit->setText(devId.toString());
	ui->deviceNameEdit->setText(QString::fromUtf8(devName));
	ui->commandEdit->setText(QString::fromUtf8(cmd));
	ui->inputsCountEdit->setValue(inCount);
	ui->argsList->clear();
	for(const QByteArray &a:args)
	{
		QListWidgetItem *item=new QListWidgetItem(QString::fromUtf8(a),ui->argsList);
		item->setFlags(item->flags()|Qt::ItemIsEditable);
	}
}

QUuid CommandBlockEditorWidget::devId()const
{
	return QUuid(ui->deviceIdEdit->text());
}

QByteArray CommandBlockEditorWidget::devName()const
{
	return ui->deviceNameEdit->text().toUtf8();
}

QByteArray CommandBlockEditorWidget::cmd()const
{
	return ui->commandEdit->text().toUtf8();
}

QByteArrayList CommandBlockEditorWidget::args()const
{
	QByteArrayList args;
	for(int i=0;i<ui->argsList->count();++i)
		args.append(ui->argsList->item(i)->text().toUtf8());
	return args;
}

quint32 CommandBlockEditorWidget::inCount()const
{
	return ui->inputsCountEdit->value();
}

void CommandBlockEditorWidget::onAddArgClicked()
{
	QListWidgetItem *item=new QListWidgetItem(ui->argsList);
	item->setFlags(item->flags()|Qt::ItemIsEditable);
	ui->argsList->editItem(item);
}

void CommandBlockEditorWidget::onDelArgClicked()
{
		if(!ui->argsList->selectedItems().contains(ui->argsList->currentItem()))
			return;
		delete ui->argsList->currentItem();
}

void CommandBlockEditorWidget::onSelectDevClicked()
{
	QUuid newId;
	QString newName;
	editor->selectDevice(newId,newName);
	if(newId.isNull())return;
	ui->deviceIdEdit->setText(newId.toString());
	ui->deviceNameEdit->setText(newName);
}
