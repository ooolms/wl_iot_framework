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
#include "ui_GroupSettingsEdit.h"
#include "ui_ControlSettingsEdit.h"
#include "ui_ParamSettingsEdit.h"

ElementSettingsWidget::ElementSettingsWidget(QWidget *parent)
	:QStackedWidget(parent)
{
	controlUi=new Ui::ControlSettingsEdit;
	groupUi=new Ui::GroupSettingsEdit;
	paramUi=new Ui::ParamSettingsEdit;

	nWidget=new QWidget(this);
	addWidget(nWidget);

	cWidget=new QWidget(this);
	addWidget(cWidget);
	controlUi->setupUi(cWidget);

	gWidget=new QWidget(this);
	addWidget(gWidget);
	groupUi->setupUi(gWidget);

	pWidget=new QWidget(this);
	addWidget(pWidget);
	paramUi->setupUi(pWidget);

	setCurrentWidget(nWidget);

	connect(paramUi->typeSelect,static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
		paramUi->typeSwitch,&QStackedWidget::setCurrentIndex);
	connect(paramUi->selectAddValueBtn,&QPushButton::clicked,this,&ElementSettingsWidget::onAddSelectValueClicked);
	connect(paramUi->selectDelValueBtn,&QPushButton::clicked,this,&ElementSettingsWidget::onDelSelectValueClicked);
	connect(paramUi->radioAddValueBtn,&QPushButton::clicked,this,&ElementSettingsWidget::onAddRadioValueClicked);
	connect(paramUi->radioDelValueBtn,&QPushButton::clicked,this,&ElementSettingsWidget::onDelRadioValueClicked);
}

ElementSettingsWidget::~ElementSettingsWidget()
{
	delete paramUi;
	delete controlUi;
	delete groupUi;
}

void ElementSettingsWidget::editGroup(ControlsGroup *group)
{
	setCurrentWidget(gWidget);
	groupUi->vLayBtn->setChecked(group->layout==Qt::Vertical);
	groupUi->hLayBtn->setChecked(group->layout==Qt::Horizontal);
}

void ElementSettingsWidget::saveGroup(ControlsGroup *group)
{
	group->layout=(groupUi->vLayBtn->isChecked()?Qt::Vertical:Qt::Horizontal);
}

void ElementSettingsWidget::saveControl(CommandControl *control)
{
	control->layout=(controlUi->vLayBtn->isChecked()?Qt::Vertical:Qt::Horizontal);
	control->command=controlUi->commandEdit->text().toUtf8();
	control->buttonText=controlUi->buttonTextEdit->text().toUtf8();
	if(control->command.isEmpty())
		control->command="";
	control->forceBtn=controlUi->forceControlBtn->isChecked();
}

//TODO config for default values
void ElementSettingsWidget::saveParam(ControlParam *param)
{
	param->type=(ControlParam::Type)(paramUi->typeSelect->currentIndex()+1);
	param->attributes.clear();
	if(param->type==ControlParam::CHECKBOX)
	{
		param->attributes["onValue"]=paramUi->checkboxOnValueEdit->text().toUtf8();
		if(param->attributes["onValue"].isEmpty())
			param->attributes.remove("onValue");
		param->attributes["offValue"]=paramUi->checkboxOffValueEdit->text().toUtf8();
		if(param->attributes["offValue"].isEmpty())
			param->attributes.remove("offValue");
	}
	else if(param->type==ControlParam::SELECT)
	{
		QByteArrayList vals,titles;
		for(int i=0;i<paramUi->selectValuesList->topLevelItemCount();++i)
		{
			QTreeWidgetItem *item=paramUi->selectValuesList->topLevelItem(i);
			vals.append(item->text(0).toUtf8());
			titles.append(item->text(1).toUtf8());
		}
		for(QByteArray &s:vals)s=s.trimmed();
		vals.removeAll(QByteArray());
		if(vals.isEmpty())
		{
			param->attributes.remove("values");
			param->attributes.remove("titles");
		}
		else
		{
			param->attributes["values"]=vals.join("|");
			param->attributes["titles"]=titles.join("|");
		}
	}
	else if(param->type==ControlParam::RADIO)
	{
		QByteArrayList vals,titles;
		for(int i=0;i<paramUi->radioValuesList->topLevelItemCount();++i)
		{
			QTreeWidgetItem *item=paramUi->radioValuesList->topLevelItem(i);
			vals.append(item->text(0).toUtf8());
			titles.append(item->text(1).toUtf8());
		}
		for(QByteArray &s:vals)s=s.trimmed();
		vals.removeAll(QByteArray());
		if(vals.isEmpty())
		{
			param->attributes.remove("values");
			param->attributes.remove("titles");
		}
		else
		{
			param->attributes["values"]=vals.join("|");
			param->attributes["titles"]=titles.join("|");
		}
	}
	else if(param->type==ControlParam::SLIDER)
	{
		if(paramUi->sliderMinValueEdit->value()==0)
			param->attributes.remove("min");
		else param->attributes["min"]=QByteArray::number(paramUi->sliderMinValueEdit->value());
		if(paramUi->sliderMaxValueEdit->value()==1023)
			param->attributes.remove("max");
		else param->attributes["max"]=QByteArray::number(paramUi->sliderMaxValueEdit->value());
		if(paramUi->sliderStepEdit->value()==1)
			param->attributes.remove("step");
		else param->attributes["step"]=QByteArray::number(paramUi->sliderStepEdit->value());
	}
	else if(param->type==ControlParam::DIAL)
	{
		if(paramUi->dialMinValueEdit->value()==0)
			param->attributes.remove("min");
		else param->attributes["min"]=QByteArray::number(paramUi->dialMinValueEdit->value());
		if(paramUi->dialMaxValueEdit->value()==1023)
			param->attributes.remove("max");
		else param->attributes["max"]=QByteArray::number(paramUi->dialMaxValueEdit->value());
		if(paramUi->dialStepEdit->value()==1)
			param->attributes.remove("step");
		else param->attributes["step"]=QByteArray::number(paramUi->dialStepEdit->value());
	}
	else if(param->type==ControlParam::HIDDEN)
	{
		if(paramUi->hiddenValue->text().isEmpty())
			param->attributes.remove("value");
		else param->attributes["value"]=paramUi->hiddenValue->text().toUtf8();
	}
}

void ElementSettingsWidget::onAddSelectValueClicked()
{
	QTreeWidgetItem *item=new QTreeWidgetItem(paramUi->selectValuesList);
	item->setFlags(item->flags()|Qt::ItemIsEditable);
	paramUi->selectValuesList->clearSelection();
	item->setSelected(true);
	paramUi->selectValuesList->editItem(item);
}

void ElementSettingsWidget::onDelSelectValueClicked()
{
	if(paramUi->selectValuesList->selectedItems().contains(paramUi->selectValuesList->currentItem()))
		delete paramUi->selectValuesList->currentItem();
}

void ElementSettingsWidget::onAddRadioValueClicked()
{
	QTreeWidgetItem *item=new QTreeWidgetItem(paramUi->radioValuesList);
	item->setFlags(item->flags()|Qt::ItemIsEditable);
	paramUi->radioValuesList->clearSelection();
	item->setSelected(true);
	paramUi->radioValuesList->editItem(item);
}

void ElementSettingsWidget::onDelRadioValueClicked()
{
	if(paramUi->radioValuesList->selectedItems().contains(paramUi->radioValuesList->currentItem()))
		delete paramUi->radioValuesList->currentItem();
}

void ElementSettingsWidget::resetAllConfigs()
{
	paramUi->checkboxOnValueEdit->setText("1");
	paramUi->checkboxOffValueEdit->setText("0");
	paramUi->selectValuesList->clear();
	paramUi->radioValuesList->clear();
	addToValuesList(paramUi->selectValuesList,"0","0");
	addToValuesList(paramUi->radioValuesList,"0","0");
	paramUi->sliderMinValueEdit->setValue(0);
	paramUi->sliderMaxValueEdit->setValue(1023);
	paramUi->sliderStepEdit->setValue(1);
	paramUi->dialMinValueEdit->setValue(0);
	paramUi->dialMaxValueEdit->setValue(1023);
	paramUi->dialStepEdit->setValue(1);
}

void ElementSettingsWidget::addToValuesList(QTreeWidget *w,const QString &val,const QString &title)
{
	QTreeWidgetItem *item=new QTreeWidgetItem(w);
	item->setText(0,val);
	item->setText(1,title);
	item->setFlags(item->flags()|Qt::ItemIsEditable);
}

void ElementSettingsWidget::editControl(CommandControl *control)
{
	setCurrentWidget(cWidget);
	controlUi->commandEdit->setText(QString::fromUtf8(control->command));
	controlUi->vLayBtn->setChecked(control->layout==Qt::Vertical);
	controlUi->hLayBtn->setChecked(control->layout==Qt::Horizontal);
	controlUi->buttonTextEdit->setText(QString::fromUtf8(control->buttonText));
	controlUi->forceControlBtn->setChecked(control->forceBtn);
}

void ElementSettingsWidget::editParam(ControlParam *param)
{
	setCurrentWidget(pWidget);
	int index=((int)param->type)-1;
	if(index==-1)index=0;
	paramUi->typeSelect->setCurrentIndex(index);
	paramUi->typeSwitch->setCurrentIndex(index);
	resetAllConfigs();
	if(param->type==ControlParam::CHECKBOX)
	{
		if(param->attributes.contains("onValue"))
			paramUi->checkboxOnValueEdit->setText(QString::fromUtf8(param->attributes["onValue"]));
		if(param->attributes.contains("offValue"))
			paramUi->checkboxOffValueEdit->setText(QString::fromUtf8(param->attributes["offValue"]));
	}
	else if(param->type==ControlParam::SELECT)
	{
		if(param->attributes.contains("values"))
		{
			paramUi->selectValuesList->clear();
			QByteArrayList vals=param->attributes["values"].split('|');
			QByteArrayList titles=param->attributes["titles"].split('|');
			bool useTitles=(titles.count()==vals.count());
			for(int i=0;i<vals.count();++i)
				addToValuesList(paramUi->selectValuesList,vals[i],useTitles?titles[i]:"");
		}
	}
	else if(param->type==ControlParam::RADIO)
	{
		if(param->attributes.contains("values"))
		{
			paramUi->radioValuesList->clear();
			QByteArrayList vals=param->attributes["values"].split('|');
			QByteArrayList titles=param->attributes["titles"].split('|');
			bool useTitles=(titles.count()==vals.count());
			for(int i=0;i<vals.count();++i)
				addToValuesList(paramUi->radioValuesList,vals[i],useTitles?titles[i]:"");
		}
	}
	else if(param->type==ControlParam::SLIDER)
	{
		if(param->attributes.contains("min"))
			paramUi->sliderMinValueEdit->setValue(param->attributes["min"].toInt());
		if(param->attributes.contains("max"))
			paramUi->sliderMaxValueEdit->setValue(param->attributes["max"].toInt());
		if(param->attributes.contains("step"))
			paramUi->sliderStepEdit->setValue(param->attributes["step"].toInt());
	}
	else if(param->type==ControlParam::DIAL)
	{
		if(param->attributes.contains("min"))
			paramUi->dialMinValueEdit->setValue(param->attributes["min"].toInt());
		if(param->attributes.contains("max"))
			paramUi->dialMaxValueEdit->setValue(param->attributes["max"].toInt());
		if(param->attributes.contains("step"))
			paramUi->dialStepEdit->setValue(param->attributes["step"].toInt());
	}
	else if(param->type==ControlParam::HIDDEN)
	{
		if(param->attributes.contains("value"))
			paramUi->hiddenValue->setText(QString::fromUtf8(param->attributes["value"]));
		else paramUi->hiddenValue->clear();
	}
}
