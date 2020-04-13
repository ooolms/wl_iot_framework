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
#include "ui_StaticSourceBlockEditorUi.h"

StaticSourceBlockEditorWidget::StaticSourceBlockEditorWidget(QWidget *parent)
	:QWidget(parent)
{
	ui=new Ui::StaticSourceBlockEditorWidget;
	ui->setupUi(this);
	ui->arrayValuesList->setDragDropMode(QAbstractItemView::InternalMove);
	connect(ui->boolBtn,&QPushButton::clicked,this,&StaticSourceBlockEditorWidget::onTypeBtnClicked);
	connect(ui->singleBtn,&QPushButton::clicked,this,&StaticSourceBlockEditorWidget::onTypeBtnClicked);
	connect(ui->arrayBtn,&QPushButton::clicked,this,&StaticSourceBlockEditorWidget::onTypeBtnClicked);
	connect(ui->arrayAddRowBtn,&QPushButton::clicked,this,&StaticSourceBlockEditorWidget::onArrAddRowClicked);
	connect(ui->arrayDelRowBtn,&QPushButton::clicked,this,&StaticSourceBlockEditorWidget::onArrDelRowClicked);
}

StaticSourceBlockEditorWidget::~StaticSourceBlockEditorWidget()
{
	delete ui;
}

DataUnit StaticSourceBlockEditorWidget::value()const
{
	if(ui->arrayBtn->isChecked())
	{
		if(ui->arrayValuesList->count()==0)
			return DataUnit(DataUnit::ARRAY,1);
		QString s=ui->arrayValuesList->item(0)->text();
		QByteArrayList b=s.toUtf8().split(';');
		if(b.isEmpty())
			return DataUnit(0.0);
		SensorDef::Type t;
		t.dim=b.count();
		t.numType=SensorDef::F64;
		t.packType=SensorDef::PACKET;
		for(int i=0;i<ui->arrayValuesList->count();++i)
		{
			s=ui->arrayValuesList->item(i)->text();
			QByteArrayList bb=s.toUtf8().split(';');
			if(bb.size()!=t.dim)
				return DataUnit(DataUnit::ARRAY,1);
			b.append(bb);
		}
		QScopedPointer<SensorValue> v(SensorValue::createSensorValue(t));
		if(!v->parseMsgArgs(b))
			return DataUnit(DataUnit::ARRAY,1);
		return DataUnit(v.data());
	}
	else if(ui->singleBtn->isChecked())
	{
		QString s=ui->singleValueEdit->text();
		QByteArrayList b=s.toUtf8().split(';');
		if(b.isEmpty())
			return DataUnit(0.0);
		SensorDef::Type t;
		t.dim=b.count();
		if(s.contains('.')||s.contains('e'))
			t.numType=SensorDef::F64;
		else t.numType=SensorDef::S64;
		QScopedPointer<SensorValue> v(SensorValue::createSensorValue(t));
		if(!v->parseMsgArgs(b))
			return DataUnit(0.0);
		return DataUnit(v.data());
	}
	else
		return DataUnit(ui->boolTrueCheck->isChecked());
}

void StaticSourceBlockEditorWidget::setValue(const DataUnit &u)
{
	ui->boolBtn->setChecked(true);
	ui->singleValueEdit->clear();
	ui->arrayValuesList->clear();
	if(u.type()==DataUnit::BOOL)
	{
		ui->boolBtn->setChecked(true);
		ui->typeEditorsSelect->setCurrentWidget(ui->boolWidget);
		ui->boolTrueCheck->setChecked(u.value()->valueToS64(0)==1);
	}
	else if(u.type()==DataUnit::SINGLE)
	{
		ui->singleBtn->setChecked(true);
		ui->typeEditorsSelect->setCurrentWidget(ui->singleWidget);
		ui->singleValueEdit->setText(valToStr(u.value(),0));
	}
	else if(u.type()==DataUnit::ARRAY)
	{
		ui->arrayBtn->setChecked(true);
		ui->typeEditorsSelect->setCurrentWidget(ui->arrayWidget);
		for(quint32 i=0;i<u.value()->packetsCount();++i)
		{
			QListWidgetItem *item=new QListWidgetItem(valToStr(u.value(),i),ui->arrayValuesList);
			item->setFlags(item->flags()|Qt::ItemIsEditable);
		}
	}
}

void StaticSourceBlockEditorWidget::onTypeBtnClicked()
{
	if(ui->boolBtn->isChecked())
		ui->typeEditorsSelect->setCurrentWidget(ui->boolWidget);
	else if(ui->singleBtn->isChecked())
		ui->typeEditorsSelect->setCurrentWidget(ui->singleWidget);
	else
		ui->typeEditorsSelect->setCurrentWidget(ui->arrayWidget);
}

void StaticSourceBlockEditorWidget::onArrAddRowClicked()
{
	QListWidgetItem *item=new QListWidgetItem(ui->arrayValuesList);
	item->setFlags(item->flags()|Qt::ItemIsEditable);
	ui->arrayValuesList->editItem(item);
}

void StaticSourceBlockEditorWidget::onArrDelRowClicked()
{
	if(!ui->arrayValuesList->selectedItems().contains(ui->arrayValuesList->currentItem()))
		return;
	delete ui->arrayValuesList->currentItem();
}

QString StaticSourceBlockEditorWidget::valToStr(const SensorValue *v,quint32 packIndex)
{
	QString retVal;
	for(quint32 i=0;i<v->type().dim;++i)
	{
		retVal+=QString::fromUtf8(v->valueToString(i,packIndex));
		retVal+=",";
	}
	retVal.chop(1);
	return retVal;
}
