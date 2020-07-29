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

#include "ProgramConfigDialog.h"
#include "ui_ProgramConfigDialog.h"
#include "VDIL/core/Program.h"
#include "VDIL/core/ProgramRuntimeVars.h"
#include "VDIL/core/ProgramVirtualDevice.h"
#include "VDIL/editor/DataUnitEdit.h"
#include <QMessageBox>
#include <QInputDialog>

using namespace WLIOT;
using namespace WLIOTUi;
using namespace WLIOTVDIL;

ProgramConfigDialog::ProgramConfigDialog(QWidget *parent)
	:QDialog(parent)
{
	ui=new Ui::ProgramConfigDialog;
	ui->setupUi(this);
	ui->runtimeVarList->setHeaderLabels(QStringList()<<"name"<<"default value");
	vdevControlsEditor=new ControlsEditor(ui->vdevTabWidget);
	ui->vdevTabWidget->addTab(vdevControlsEditor,"Controls");
	vdevSensorsEditor=new SensorsEditor(QList<SensorDef::NumType>()<<SensorDef::F64,false,false,ui->vdevTabWidget);
	ui->vdevTabWidget->addTab(vdevSensorsEditor,"Sensors list");

	connect(ui->addRuntimeVarBtn,&QPushButton::clicked,this,&ProgramConfigDialog::onAddRuntimeVarClicked);
	connect(ui->delRuntimeVarBtn,&QPushButton::clicked,this,&ProgramConfigDialog::onDelRuntimeVarClicked);
	connect(ui->editRuntimeVarBtn,&QPushButton::clicked,this,&ProgramConfigDialog::onEditRuntimeVarClicked);
	connect(ui->renameRuntimeVarBtn,&QPushButton::clicked,this,&ProgramConfigDialog::onRenameRuntimeVarClicked);
	connect(ui->vdevGenIdBtn,&QPushButton::clicked,this,&ProgramConfigDialog::onGenerateIdClicked);
}

ProgramConfigDialog::~ProgramConfigDialog()
{
	delete ui;
}

void ProgramConfigDialog::loadSettings(const Program *p)
{
	ui->runtimeVarList->clear();
	mVarNames=p->runtimeVars()->allVars();
	mVarDefVals.clear();
	for(int i=0;i<mVarNames.count();++i)
	{
		QString &n=mVarNames[i];
		DataUnit v=p->runtimeVars()->defaultValue(n);
		mVarDefVals.append(v);
		QTreeWidgetItem *item=new QTreeWidgetItem(ui->runtimeVarList);
		item->setText(0,n);
		item->setText(1,dataUnitHint(v));
	}
	vdevSensorsEditor->setSensors(p->vdev()->sensors());
	vdevControlsEditor->setControls(p->vdev()->controls());
	ui->vdevEnableCheck->setChecked(p->vdev()->enabled());
	ui->vdevIdEdit->setText(p->vdev()->devId().toString());
	ui->vdevNameEdit->setText(QString::fromUtf8(p->vdev()->devName()));
}

void ProgramConfigDialog::storeSettings(Program *p)
{
	QStringList oldVars=p->runtimeVars()->allVars();
	for(QString &n:oldVars)
		if(!mVarNames.contains(n))
			p->runtimeVars()->removeVar(n);
	for(int i=0;i<mVarNames.count();++i)
	{
		QString &n=mVarNames[i];
		DataUnit &v=mVarDefVals[i];
		if(oldVars.contains(n)&&p->runtimeVars()->defaultValue(n)==v)
			continue;
		p->runtimeVars()->setupVar(n,v);
	}
	p->vdev()->setSensors(vdevSensorsEditor->sensors());
	p->vdev()->setControls(vdevControlsEditor->controls());
	p->vdev()->setParams(ui->vdevEnableCheck->isChecked(),
		QUuid(ui->vdevIdEdit->text()),ui->vdevNameEdit->text().toUtf8());
}

void ProgramConfigDialog::onAddRuntimeVarClicked()
{
	QString name=QInputDialog::getText(this,"Variable name","name");
	if(name.isEmpty())return;
	if(mVarNames.contains(name))
	{
		QMessageBox::warning(this,"Variable not added","Variable with this name already exists");
		return;
	}
	DataUnit v(1.0);
	auto dlg=DataUnitEdit::mkDialog(TypeConstraints(DataUnit::ANY,0),this);
	dlg.second->setValue(v);
	if(dlg.first->exec()!=QDialog::Accepted)
	{
		delete dlg.first;
		return;
	}
	v=dlg.second->value();
	mVarNames.append(name);
	mVarDefVals.append(v);
	QTreeWidgetItem *item=new QTreeWidgetItem(ui->runtimeVarList);
	item->setText(0,name);
	item->setText(1,dataUnitHint(v));
	delete dlg.first;
}

void ProgramConfigDialog::onDelRuntimeVarClicked()
{
	int index=selectedVarIndex();
	if(index==-1)return;
	if(QMessageBox::question(this,"Sure?","Remove runtime variable?")!=QMessageBox::Yes)return;
	mVarNames.removeAt(index);
	mVarDefVals.removeAt(index);
	delete ui->runtimeVarList->topLevelItem(index);
}

void ProgramConfigDialog::onEditRuntimeVarClicked()
{
	int index=selectedVarIndex();
	if(index==-1)return;
	DataUnit &v=mVarDefVals[index];
	auto dlg=DataUnitEdit::mkDialog(TypeConstraints(DataUnit::ANY,0),this);
	dlg.second->setValue(v);
	if(dlg.first->exec()!=QDialog::Accepted)
	{
		delete dlg.first;
		return;
	}
	v=dlg.second->value();
	QTreeWidgetItem *item=ui->runtimeVarList->topLevelItem(index);
	item->setText(1,dataUnitHint(v));
	delete dlg.first;
}

void ProgramConfigDialog::onRenameRuntimeVarClicked()
{
	int index=selectedVarIndex();
	if(index==-1)return;
	QString &n=mVarNames[index];
	QString newName=QInputDialog::getText(this,"Variable name","name",QLineEdit::Normal,n);
	if(newName.isEmpty())return;
	if(mVarNames.contains(newName))
	{
		QMessageBox::warning(this,"Variable not renamed","Variable with new name already exists");
		return;
	}
	n=newName;
	QTreeWidgetItem *item=ui->runtimeVarList->topLevelItem(index);
	item->setText(0,newName);
}

void ProgramConfigDialog::onGenerateIdClicked()
{
	ui->vdevIdEdit->setText(QUuid::createUuid().toString());
}

QString ProgramConfigDialog::dataUnitHint(const DataUnit &v)
{
	QString s=QString::fromUtf8(DataUnit::typeToStr(v.type()));
	if(v.type()==DataUnit::ARRAY)
		s+=":[...]";
	else if(v.type()==DataUnit::SINGLE)
		s+=":("+QString::fromUtf8(v.value()->dumpToMsgArgs().join(','))+")";
	else s+=QString::fromUtf8(":")+QString::fromUtf8(v.value()->valueToS64(0)==1?"true":"false");
	return s;
}

int ProgramConfigDialog::selectedVarIndex()
{
	if(!ui->runtimeVarList->selectedItems().contains(ui->runtimeVarList->currentItem()))return -1;
	QTreeWidgetItem *item=ui->runtimeVarList->currentItem();
	return ui->runtimeVarList->indexOfTopLevelItem(item);
}
