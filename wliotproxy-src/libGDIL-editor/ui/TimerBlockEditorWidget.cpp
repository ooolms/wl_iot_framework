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

#include "TimerBlockEditorWidget.h"
#include "ui_TimerBlockEditorWidget.h"

TimerBlockEditorWidget::TimerBlockEditorWidget(QWidget *parent)
	:QWidget(parent)
{
	ui=new Ui::TimerBlockEditorWidget;
	ui->setupUi(this);

	grp.addButton(ui->singleBtn);
	grp.addButton(ui->secBtn);
	grp.addButton(ui->minBtn);
	grp.addButton(ui->hourBtn);
	grp.addButton(ui->daysBtn);
	grp.addButton(ui->monthBtn);
	grp.addButton(ui->yearlyBtn);
}

TimerBlockEditorWidget::~TimerBlockEditorWidget()
{
	delete ui;
}

TimerBlock::TimerConfig TimerBlockEditorWidget::config()const
{
	TimerBlock::TimerConfig cfg;
	if(ui->secBtn->isChecked())
		cfg.policy=TimerBlock::REGULAR_SEC;
	else if(ui->minBtn->isChecked())
		cfg.policy=TimerBlock::REGULAR_MIN;
	else if(ui->hourBtn->isChecked())
		cfg.policy=TimerBlock::REGULAR_HOUR;
	else if(ui->daysBtn->isChecked())
		cfg.policy=TimerBlock::REGULAR_DAY;
	else if(ui->monthBtn->isChecked())
		cfg.policy=TimerBlock::MONTH;
	else if(ui->yearlyBtn->isChecked())
		cfg.policy=TimerBlock::YEAR;
	else cfg.policy=TimerBlock::SINGLE;
	cfg.startTime=ui->startTimeEdit->dateTime();
	cfg.repeatInterval=ui->repeatIntervalEdit->value();
	if(cfg.repeatInterval<=0)
		cfg.repeatInterval=1;
	return cfg;
}

bool TimerBlockEditorWidget::configurable()
{
	return ui->configurableCheck->isChecked();
}

void TimerBlockEditorWidget::setConfig(const TimerBlock::TimerConfig &cfg,bool configurable)
{
	if(cfg.policy==TimerBlock::REGULAR_SEC)
		ui->secBtn->setChecked(true);
	else if(cfg.policy==TimerBlock::REGULAR_MIN)
		ui->minBtn->setChecked(true);
	else if(cfg.policy==TimerBlock::REGULAR_HOUR)
		ui->hourBtn->setChecked(true);
	else if(cfg.policy==TimerBlock::REGULAR_DAY)
		ui->daysBtn->setChecked(true);
	else if(cfg.policy==TimerBlock::MONTH)
		ui->monthBtn->setChecked(true);
	else if(cfg.policy==TimerBlock::YEAR)
		ui->yearlyBtn->setChecked(true);
	else ui->singleBtn->setChecked(true);
	ui->startTimeEdit->setDateTime(cfg.startTime);
	ui->repeatIntervalEdit->setValue(cfg.repeatInterval);
	ui->configurableCheck->setChecked(configurable);
}
