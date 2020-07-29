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

#include "VDIL/editor/DataUnitTypeEdit.h"
#include "ui_DataUnitTypeEdit.h"
#include <QDialog>
#include <QDialogButtonBox>
#include <QLayout>

using namespace WLIOT;
using namespace WLIOTVDIL;

DataUnitTypeEdit::DataUnitTypeEdit(const TypeConstraints &c,QWidget *parent)
	:QWidget(parent)
{
	mConstr=c;
	ui=new Ui::DataUnitTypeEdit;
	ui->setupUi(this);
	connect(ui->boolBtn,&QRadioButton::clicked,this,&DataUnitTypeEdit::onTypeBtnClicked);
	connect(ui->singleBtn,&QRadioButton::clicked,this,&DataUnitTypeEdit::onTypeBtnClicked);
	connect(ui->arrayBtn,&QRadioButton::clicked,this,&DataUnitTypeEdit::onTypeBtnClicked);
	connect(ui->dateBtn,&QRadioButton::clicked,this,&DataUnitTypeEdit::onTypeBtnClicked);

	if(!(mConstr.types&DataUnit::SINGLE))
		ui->singleBtn->setEnabled(false);
	if(!(mConstr.types&DataUnit::ARRAY))
		ui->arrayBtn->setEnabled(false);
	if(!(mConstr.types&DataUnit::BOOL))
		ui->boolBtn->setEnabled(false);
	if(!(mConstr.types&DataUnit::DATETIME))
		ui->dateBtn->setEnabled(false);

	if(mConstr.dim!=0)
	{
		ui->dimEdit->setValue(mConstr.dim);
		ui->dimEdit->setEnabled(false);
	}
}

QPair<QDialog*,DataUnitTypeEdit*> DataUnitTypeEdit::mkDialog(const TypeConstraints &c,QWidget *parent)
{
	QPair<QDialog*,DataUnitTypeEdit*> retVal;
	retVal.first=new QDialog(parent);
	QDialogButtonBox *btns=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,retVal.first);
	retVal.second=new DataUnitTypeEdit(c,retVal.first);
	QVBoxLayout *lay=new QVBoxLayout(retVal.first);
	lay->addWidget(retVal.second,1);
	lay->addWidget(btns);
	connect(btns,&QDialogButtonBox::accepted,retVal.first,&QDialog::accept);
	connect(btns,&QDialogButtonBox::rejected,retVal.first,&QDialog::reject);
	return retVal;
}

DataUnitTypeEdit::~DataUnitTypeEdit()
{
	delete ui;
}

TypeAndDim DataUnitTypeEdit::type()
{
	TypeAndDim td;
	if(ui->arrayBtn->isChecked()||ui->singleBtn->isChecked())
		td.dim=ui->dimEdit->value();
	else td.dim=1;
	if(ui->arrayBtn->isChecked())
		td.type=DataUnit::ARRAY;
	else if(ui->singleBtn->isChecked())
		td.type=DataUnit::SINGLE;
	else if(ui->dateBtn->isChecked())
		td.type=DataUnit::DATETIME;
	else  td.type=DataUnit::BOOL;
	return td;
}

void DataUnitTypeEdit::setType(TypeAndDim t)
{
	if(!mConstr.match(t))
		return;
	ui->dimEdit->setValue(t.dim);
	if(t.type==DataUnit::DATETIME)
		ui->dateBtn->setChecked(true);
	else if(t.type==DataUnit::SINGLE)
		ui->singleBtn->setChecked(true);
	else if(t.type==DataUnit::ARRAY)
		ui->arrayBtn->setChecked(true);
	else ui->boolBtn->setChecked(true);
}

void DataUnitTypeEdit::onTypeBtnClicked()
{
	if(mConstr.dim==0)
		ui->dimEdit->setEnabled(!ui->boolBtn->isChecked()&&!ui->dateBtn->isChecked());
}
