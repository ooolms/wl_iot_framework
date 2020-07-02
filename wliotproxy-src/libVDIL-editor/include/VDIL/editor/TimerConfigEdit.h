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

#ifndef TIMERCONFIGEDIT_H
#define TIMERCONFIGEDIT_H

#include <QWidget>
#include <QButtonGroup>
#include "VDIL/core/TimerBlock.h"

namespace Ui
{
	class TimerConfigEdit;
}

namespace WLIOTVDIL
{
	class TimerConfigEdit
		:public QWidget
	{
		Q_OBJECT
	public:
		explicit TimerConfigEdit(QWidget *parent=0);
		virtual ~TimerConfigEdit();
		void setConfig(const TimerBlock::TimerConfig &cfg);
		TimerBlock::TimerConfig config();

	private:
		Ui::TimerConfigEdit *ui;
		QButtonGroup grp;
	};
}

#endif // TIMERCONFIGEDIT_H