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

#ifndef ELEMENTSETTINGSWIDGET_H
#define ELEMENTSETTINGSWIDGET_H

#include <QStackedWidget>
#include <QTreeWidgetItem>
#include "wliot/devices/ControlsDefinition.h"

namespace Ui
{
	class ControlSettingsEdit;
	class GroupSettingsEdit;
	class ParamSettingsEdit;
}

namespace WLIOTUi
{
	class ElementSettingsWidget
		:public QStackedWidget
	{
		Q_OBJECT
	public:
		explicit ElementSettingsWidget(QWidget *parent=0);
		virtual ~ElementSettingsWidget();
		void editNothing();
		void editGroup(WLIOT::ControlsGroup *group);
		void editControl(WLIOT::CommandControl *control);
		void editParam(WLIOT::ControlParam *param);
		void saveGroup(WLIOT::ControlsGroup *group);
		void saveControl(WLIOT::CommandControl *control);
		void saveParam(WLIOT::ControlParam *param);

	private slots:
		void onAddSelectValueClicked();
		void onDelSelectValueClicked();
		void onAddRadioValueClicked();
		void onDelRadioValueClicked();

	private:
		void resetAllConfigs();
		void addToValuesList(QTreeWidget *w,const QString &val,const QString &title);

	private:
		QWidget *nWidget,*cWidget,*gWidget,*pWidget;
		Ui::ControlSettingsEdit *controlUi;
		Ui::GroupSettingsEdit *groupUi;
		Ui::ParamSettingsEdit *paramUi;
	};
}

#endif // ELEMENTSETTINGSWIDGET_H
