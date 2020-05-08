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

#include "GDIL/editor/DataUnitEdit.h"
#include "DataUnitValueEdit.h"
#include "ui_DataUnitEdit.h"

DataUnitEdit::DataUnitEdit(const TypeConstraints &c,QWidget *parent)
	:QWidget(parent)
{
	mConstr=c;
	ui=new Ui::DataUnitEdit;
	ui->setupUi(this);
	singleValueEdit=new DataUnitValueEdit(ui->singleWidget);
	((QVBoxLayout*)ui->singleWidget->layout())->insertWidget(1,singleValueEdit);
	ui->arrayValuesList->setDragDropMode(QAbstractItemView::InternalMove);
	connect(ui->boolBtn,&QPushButton::clicked,this,&DataUnitEdit::onTypeBtnClicked);
	connect(ui->singleBtn,&QPushButton::clicked,this,&DataUnitEdit::onTypeBtnClicked);
	connect(ui->arrayBtn,&QPushButton::clicked,this,&DataUnitEdit::onTypeBtnClicked);
	connect(ui->arrayAddRowBtn,&QPushButton::clicked,this,&DataUnitEdit::onArrAddRowClicked);
	connect(ui->arrayDelRowBtn,&QPushButton::clicked,this,&DataUnitEdit::onArrDelRowClicked);
	connect(ui->dimEdit,static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		this,&DataUnitEdit::onDimValueChanged);

	if(!(mConstr.types&DataUnit::SINGLE))
		ui->singleBtn->setEnabled(false);
	if(!(mConstr.types&DataUnit::ARRAY))
		ui->arrayBtn->setEnabled(false);
	if(!(mConstr.types&DataUnit::BOOL))
		ui->boolBtn->setEnabled(false);

	if(mConstr.dim!=0)
	{
		ui->dimEdit->setValue(mConstr.dim);
		ui->dimEdit->setEnabled(false);
	}
	onDimValueChanged();
	if(mConstr.types&DataUnit::BOOL)
		setValue(DataUnit(false));
	else if(mConstr.types&DataUnit::SINGLE)
		setValue(DataUnit(DataUnit::SINGLE,ui->dimEdit->value()));
	else if(mConstr.types&DataUnit::ARRAY)
		setValue(DataUnit(DataUnit::ARRAY,ui->dimEdit->value()));
	else ui->boolBtn->setChecked(false);
}

DataUnitEdit::~DataUnitEdit()
{
	delete ui;
}

DataUnit DataUnitEdit::value()const
{
	quint32 dim=ui->dimEdit->value();
	if(ui->arrayBtn->isChecked())
	{
		if(ui->arrayValuesList->topLevelItemCount()==0)
			return DataUnit(DataUnit::ARRAY,dim);
		bool hasFloatVals=false;
		for(int i=0;i<ui->arrayValuesList->topLevelItemCount();++i)
		{
			DataUnitValueEdit *edit=((DataUnitValueEdit*)ui->arrayValuesList->itemWidget(
				ui->arrayValuesList->topLevelItem(i),0));
			if(!edit->canBeS64())
			{
				hasFloatVals=false;
				break;
			}
		}
		SensorDef::Type t;
		t.dim=dim;
		t.numType=(hasFloatVals?SensorDef::F64:SensorDef::S64);
		t.packType=SensorDef::PACKET;
		QScopedPointer<SensorValue> v(SensorValue::createSensorValue(t));
		for(int i=0;i<ui->arrayValuesList->topLevelItemCount();++i)
		{
			DataUnitValueEdit *edit=((DataUnitValueEdit*)ui->arrayValuesList->itemWidget(
				ui->arrayValuesList->topLevelItem(i),0));
			if(hasFloatVals)
				((SensorValueF64*)v.data())->addSample(edit->f64Values());
			else ((SensorValueS64*)v.data())->addSample(edit->s64Values());

		}
		return DataUnit(v.data());
	}
	else if(ui->singleBtn->isChecked())
	{
		if(singleValueEdit->canBeS64())
			return DataUnit(singleValueEdit->s64Values());
		else return DataUnit(singleValueEdit->f64Values());
	}
	else
		return DataUnit(ui->boolTrueCheck->isChecked());
}

void DataUnitEdit::setValue(const DataUnit &v)
{
	if(!mConstr.match(v))
		return;
	ui->boolBtn->setChecked(true);
	singleValueEdit->setDim(0);
	ui->arrayValuesList->clear();
	quint32 dim=v.dim();
	ui->dimEdit->setValue(dim);
	singleValueEdit->setDim(dim);
	if(v.type()==DataUnit::BOOL)
	{
		ui->boolBtn->setChecked(true);
		ui->typeEditorsSelect->setCurrentWidget(ui->boolWidget);
		ui->boolTrueCheck->setChecked(v.value()->valueToS64(0)==1);
	}
	else if(v.type()==DataUnit::SINGLE)
	{
		ui->singleBtn->setChecked(true);
		ui->typeEditorsSelect->setCurrentWidget(ui->singleWidget);
		if(v.numType()==DataUnit::F64)
			singleValueEdit->setValues(((const SensorValueF64*)v.value())->data());
		else singleValueEdit->setValues(((const SensorValueS64*)v.value())->data());
	}
	else if(v.type()==DataUnit::ARRAY)
	{
		ui->arrayBtn->setChecked(true);
		ui->typeEditorsSelect->setCurrentWidget(ui->arrayWidget);
		for(quint32 i=0;i<v.value()->packetsCount();++i)
		{
			QTreeWidgetItem *item=new QTreeWidgetItem(ui->arrayValuesList);
			DataUnitValueEdit *e=new DataUnitValueEdit(ui->arrayValuesList);
			e->setDim(dim);
			if(v.numType()==DataUnit::F64)
				e->setValues(((const SensorValueF64*)v.value())->getSample(i));
			else e->setValues(((const SensorValueS64*)v.value())->getSample(i));
			ui->arrayValuesList->setItemWidget(item,0,e);
		}
	}
}

void DataUnitEdit::onTypeBtnClicked()
{
	if(ui->boolBtn->isChecked())
		ui->typeEditorsSelect->setCurrentWidget(ui->boolWidget);
	else if(ui->singleBtn->isChecked())
		ui->typeEditorsSelect->setCurrentWidget(ui->singleWidget);
	else ui->typeEditorsSelect->setCurrentWidget(ui->arrayWidget);
	if(mConstr.dim==0)
		ui->dimEdit->setEnabled(!ui->boolBtn->isChecked());
}

void DataUnitEdit::onArrAddRowClicked()
{
	QTreeWidgetItem *item=new QTreeWidgetItem(ui->arrayValuesList);
	item->setFlags(item->flags()|Qt::ItemIsEditable);
	ui->arrayValuesList->editItem(item);
	DataUnitValueEdit *e=new DataUnitValueEdit(ui->arrayValuesList);
	e->setDim(ui->dimEdit->value());
	ui->arrayValuesList->setItemWidget(item,0,e);
}

void DataUnitEdit::onArrDelRowClicked()
{
	if(!ui->arrayValuesList->selectedItems().contains(ui->arrayValuesList->currentItem()))
		return;
	delete ui->arrayValuesList->currentItem();
}

void DataUnitEdit::onDimValueChanged()
{
	quint32 dim=ui->dimEdit->value();
	singleValueEdit->setDim(dim);
	for(int i=0;i<ui->arrayValuesList->topLevelItemCount();++i)
		((DataUnitValueEdit*)ui->arrayValuesList->itemWidget(ui->arrayValuesList->topLevelItem(i),0))->setDim(dim);
}
