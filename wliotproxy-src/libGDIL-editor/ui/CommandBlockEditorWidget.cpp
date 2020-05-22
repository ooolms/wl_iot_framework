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
#include "ui_CommandBlockEditorWidget.h"
#include "EditorInternalApi.h"
#include <QUuid>
#include <QDialog>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QLayout>

using namespace WLIOT;
using namespace WLIOTGDIL;

CommandBlockEditorWidget::CommandBlockEditorWidget(IEditorHelper *helper,QWidget *parent)
	:QWidget(parent)
{
	mHelper=helper;
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

void CommandBlockEditorWidget::setParams(const QUuid &devId,const QString &devName,const QByteArray &cmd,
	const QByteArrayList &args,quint32 inCount,bool enableConditionInput)
{
	ui->deviceIdEdit->setText(devId.toString());
	ui->commandEdit->setText(QString::fromUtf8(cmd));
	ui->deviceNameEdit->setText(devName);
	ui->inputsCountEdit->setValue(inCount);
	ui->argsList->clear();
	ui->conditionalInputCheck->setChecked(enableConditionInput);
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

QString CommandBlockEditorWidget::devName()const
{
	return ui->deviceNameEdit->text();
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

bool CommandBlockEditorWidget::enableConditionInput()const
{
	return ui->conditionalInputCheck->isChecked();
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
	ControlsGroup ctls;
	if(!mHelper->selectDevice(newId,newName,ctls))return;
	if(newId.isNull())return;
	ui->deviceIdEdit->setText(newId.toString());
	ui->deviceNameEdit->setText(newName);

	QList<CommandControl> cmds=ctls.extractCommandsList();
	if(cmds.isEmpty())return;
	QDialog dlg;
	QDialogButtonBox *btns=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,&dlg);
	QListWidget *cmdList=new QListWidget(&dlg);

	QVBoxLayout *lay=new QVBoxLayout(&dlg);
	lay->addWidget(cmdList,1);
	lay->addWidget(btns);

	connect(btns,&QDialogButtonBox::accepted,&dlg,&QDialog::accept);
	connect(btns,&QDialogButtonBox::rejected,&dlg,&QDialog::reject);

	for(CommandControl &c:cmds)
		cmdList->addItem(QString::fromUtf8(c.command+": "+c.title));
	if(dlg.exec()!=QDialog::Accepted)return;
	if(!cmdList->selectedItems().contains(cmdList->currentItem()))
		return;
	CommandControl c=cmds.value(cmdList->currentRow());
	ui->commandEdit->setText(QString::fromUtf8(c.command));
	ui->argsList->clear();
	for(int i=0;i<c.params.count();++i)
	{
		QListWidgetItem *item=new QListWidgetItem(ui->argsList);
		item->setFlags(item->flags()|Qt::ItemIsEditable);
	}
}
