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

#include "ElementSettingsWidget.h"

ElementSettingsWidget::ElementSettingsWidget(QWidget *parent)
	:QStackedWidget(parent)
{
	nWidget=new QWidget(this);
	addWidget(nWidget);

	cWidget=new QWidget(this);
	addWidget(cWidget);
	controlUi.setupUi(cWidget);

	gWidget=new QWidget(this);
	addWidget(gWidget);
	groupUi.setupUi(gWidget);

	pWidget=new QWidget(this);
	addWidget(pWidget);
	paramUi.setupUi(pWidget);

	setCurrentWidget(nWidget);

	connect(paramUi.typeSelect,static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
		paramUi.typeSwitch,&QStackedWidget::setCurrentIndex);
	connect(paramUi.selectAddValueBtn,&QPushButton::clicked,this,&ElementSettingsWidget::onAddSelectValueClicked);
	connect(paramUi.selectDelValueBtn,&QPushButton::clicked,this,&ElementSettingsWidget::onDelSelectValueClicked);
}

void ElementSettingsWidget::editGroup(ARpcControlsGroup *group)
{
	setCurrentWidget(gWidget);
	groupUi.vLayBtn->setChecked(group->layout==Qt::Vertical);
	groupUi.hLayBtn->setChecked(group->layout==Qt::Horizontal);
}

void ElementSettingsWidget::saveGroup(ARpcControlsGroup *group)
{
	group->layout=(groupUi.vLayBtn->isChecked()?Qt::Vertical:Qt::Horizontal);
}

void ElementSettingsWidget::saveControl(ARpcCommandControl *control)
{
	control->layout=(controlUi.vLayBtn->isChecked()?Qt::Vertical:Qt::Horizontal);
	control->command=controlUi.commandEdit->text().toUtf8();
	if(control->command.isEmpty())control->command="";
	control->syncCall=controlUi.syncCallCheck->isChecked();
}

//TODO config for default values
void ElementSettingsWidget::saveParam(ARpcControlParam *param)
{
	param->type=(ARpcControlParam::Type)(paramUi.typeSelect->currentIndex()+1);
	param->constraints.clear();
	if(param->type==ARpcControlParam::CHECKBOX)
	{
		param->constraints["onValue"]=paramUi.checkboxOnValueEdit->text().toUtf8();
		if(param->constraints["onValue"].isEmpty())
			param->constraints.remove("onValue");
		param->constraints["offValue"]=paramUi.checkboxOffValueEdit->text().toUtf8();
		if(param->constraints["offValue"].isEmpty())
			param->constraints.remove("offValue");
	}
	else if(param->type==ARpcControlParam::SELECT)
	{
		QByteArrayList vals;
		for(int i=0;i<paramUi.selectValuesList->count();++i)
			vals.append(paramUi.selectValuesList->item(i)->text().toUtf8());
		for(QByteArray &s:vals)s=s.trimmed();
		vals.removeAll(QByteArray());
		if(vals.isEmpty())param->constraints.remove("values");
		else param->constraints["values"]=vals.join(";");
	}
	else if(param->type==ARpcControlParam::SLIDER)
	{
		if(paramUi.sliderMinValueEdit->value()==0)
			param->constraints.remove("min");
		else param->constraints["min"]=QByteArray::number(paramUi.sliderMinValueEdit->value());
		if(paramUi.sliderMaxValueEdit->value()==1023)
			param->constraints.remove("max");
		else param->constraints["max"]=QByteArray::number(paramUi.sliderMaxValueEdit->value());
		if(paramUi.sliderStepEdit->value()==1)
			param->constraints.remove("step");
		else param->constraints["step"]=QByteArray::number(paramUi.sliderStepEdit->value());
	}
	else if(param->type==ARpcControlParam::DIAL)
	{
		if(paramUi.dialMinValueEdit->value()==0)
			param->constraints.remove("min");
		else param->constraints["min"]=QByteArray::number(paramUi.dialMinValueEdit->value());
		if(paramUi.dialMaxValueEdit->value()==1023)
			param->constraints.remove("max");
		else param->constraints["max"]=QByteArray::number(paramUi.dialMaxValueEdit->value());
		if(paramUi.dialStepEdit->value()==1)
			param->constraints.remove("step");
		else param->constraints["step"]=QByteArray::number(paramUi.dialStepEdit->value());
	}
}

void ElementSettingsWidget::onAddSelectValueClicked()
{
	QListWidgetItem *item=new QListWidgetItem(paramUi.selectValuesList);
	item->setFlags(item->flags()|Qt::ItemIsEditable);
	paramUi.selectValuesList->clearSelection();
	item->setSelected(true);
	paramUi.selectValuesList->editItem(item);
}

void ElementSettingsWidget::onDelSelectValueClicked()
{
	if(paramUi.selectValuesList->selectedItems().contains(paramUi.selectValuesList->currentItem()))
		delete paramUi.selectValuesList->currentItem();
}

void ElementSettingsWidget::resetAllConfigs()
{
	paramUi.checkboxOnValueEdit->setText("1");
	paramUi.checkboxOffValueEdit->setText("0");
	paramUi.selectValuesList->clear();
	addToSelectValuesList("0");
	paramUi.sliderMinValueEdit->setValue(0);
	paramUi.sliderMaxValueEdit->setValue(1023);
	paramUi.sliderStepEdit->setValue(1);
	paramUi.dialMinValueEdit->setValue(0);
	paramUi.dialMaxValueEdit->setValue(1023);
	paramUi.dialStepEdit->setValue(1);
}

void ElementSettingsWidget::addToSelectValuesList(const QString &str)
{
	QListWidgetItem *item=new QListWidgetItem(paramUi.selectValuesList);
	item->setText(str);
	item->setFlags(item->flags()|Qt::ItemIsEditable);
}

void ElementSettingsWidget::editControl(ARpcCommandControl *control)
{
	setCurrentWidget(cWidget);
	controlUi.commandEdit->setText(QString::fromUtf8(control->command));
	controlUi.vLayBtn->setChecked(control->layout==Qt::Vertical);
	controlUi.hLayBtn->setChecked(control->layout==Qt::Horizontal);
	controlUi.syncCallCheck->setChecked(control->syncCall);
}

void ElementSettingsWidget::editParam(ARpcControlParam *param)
{
	setCurrentWidget(pWidget);
	int index=((int)param->type)-1;
	if(index==-1)index=0;
	paramUi.typeSelect->setCurrentIndex(index);
	paramUi.typeSwitch->setCurrentIndex(index);
	resetAllConfigs();
	if(param->type==ARpcControlParam::CHECKBOX)
	{
		if(param->constraints.contains("onValue"))
			paramUi.checkboxOnValueEdit->setText(QString::fromUtf8(param->constraints["onValue"]));
		if(param->constraints.contains("offValue"))
			paramUi.checkboxOffValueEdit->setText(QString::fromUtf8(param->constraints["offValue"]));
	}
	else if(param->type==ARpcControlParam::SELECT)
	{
		if(param->constraints.contains("values"))
		{
			paramUi.selectValuesList->clear();
			QByteArrayList vals=param->constraints["values"].split(';');
			for(QByteArray &s:vals)addToSelectValuesList(s);
		}
	}
	else if(param->type==ARpcControlParam::SLIDER)
	{
		if(param->constraints.contains("min"))
			paramUi.sliderMinValueEdit->setValue(param->constraints["min"].toInt());
		if(param->constraints.contains("max"))
			paramUi.sliderMaxValueEdit->setValue(param->constraints["max"].toInt());
		if(param->constraints.contains("step"))
			paramUi.sliderStepEdit->setValue(param->constraints["step"].toInt());
	}
	else if(param->type==ARpcControlParam::DIAL)
	{
		if(param->constraints.contains("min"))
			paramUi.dialMinValueEdit->setValue(param->constraints["min"].toInt());
		if(param->constraints.contains("max"))
			paramUi.dialMaxValueEdit->setValue(param->constraints["max"].toInt());
		if(param->constraints.contains("step"))
			paramUi.dialStepEdit->setValue(param->constraints["step"].toInt());
	}
}
