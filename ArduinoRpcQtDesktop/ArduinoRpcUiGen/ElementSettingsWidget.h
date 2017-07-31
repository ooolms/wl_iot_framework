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
#include "ARpcBase/ARpcControlsDefinition.h"
#include "ui_GroupSettingsEdit.h"
#include "ui_ControlSettingsEdit.h"
#include "ui_ParamSettingsEdit.h"

class ElementSettingsWidget
	:public QStackedWidget
{
	Q_OBJECT
public:
	explicit ElementSettingsWidget(QWidget *parent=0);
	void editNothing();
	void editGroup(ARpcControlsGroup *group);
	void editControl(ARpcCommandControl *control);
	void editParam(ARpcControlParam *param);
	void saveGroup(ARpcControlsGroup *group);
	void saveControl(ARpcCommandControl *control);
	void saveParam(ARpcControlParam *param);

private slots:
	void onAddSelectValueClicked();
	void onDelSelectValueClicked();

private:
	void resetAllConfigs();
	void addToSelectValuesList(const QString &str);

private:
	QWidget *nWidget,*cWidget,*gWidget,*pWidget;
	Ui::ControlSettingsEdit controlUi;
	Ui::GroupSettingsEdit groupUi;
	Ui::ParamSettingsEdit paramUi;
};

#endif // ELEMENTSETTINGSWIDGET_H
