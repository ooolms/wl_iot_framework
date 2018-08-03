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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_MainWindow.h"
#include "ElementSettingsWidget.h"
#include "ARpcBase/ARpcControlsDefinition.h"
#include "ARpcUi/ARpcControlUi.h"
#include "FakeDevice.h"

class MainWindow
	:public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent=0);

private slots:
	void onUiTreeSelChanged();
	void onSensorsTreeSelChanged();
	void onAddUiGroupClicked();
	void onAddUiControlClicked();
	void onAddUiParamClicked();
	void onDelUiElementClicked();
	void onSaveUiAsXmlTriggered();
	void onSaveUiAsJsonTriggered();
	void onOpenUiXmlTriggered();
	void onOpenUiJsonTriggered();
	void onCopyUiXmlAsVarTriggered();
	void onCopyUiJsonAsVarTriggered();
	void onSaveSensorsAsXmlTriggered();
	void onSaveSensorsAsJsonTriggered();
	void onOpenSensorsXmlTriggered();
	void onOpenSensorsJsonTriggered();
	void onCopySensorsXmlAsVarTriggered();
	void onCopySensorsJsonAsVarTriggered();
	void onLogMsg(const QString &msg);
	void onUiTreeItemEdited();
	void onAddSensorClicked();
	void onDelSensorClicked();
	void onGenUuidTriggered();

private:
	QTreeWidgetItem* getCurrentUiGroup();
	QTreeWidgetItem* getCurrentUiControl();
	QTreeWidgetItem* getCurrentUiParam();
	QTreeWidgetItem* mkUiGroupItem(QTreeWidgetItem *parent,const ARpcControlsGroup &g=ARpcControlsGroup());
	QTreeWidgetItem* mkUiControlItem(QTreeWidgetItem *parent,const ARpcCommandControl &c=ARpcCommandControl());
	QTreeWidgetItem* mkUiParamItem(QTreeWidgetItem *parent,const ARpcControlParam &p=ARpcControlParam());
	void dumpUiGroup(QTreeWidgetItem *item,ARpcControlsGroup &g);
	void dumpUiCommand(QTreeWidgetItem *item,ARpcCommandControl &c);
	void dumpSensors(QList<ARpcSensorDef> &sensors);
	void saveCurrentEditedUiItem();
	void saveCurrentEditedSensorsItem();
	void rebuildControlUi();
	void buildSensorsList(const QList<ARpcSensorDef> &sensors);

private:
	Ui::MainWindow ui;
	ElementSettingsWidget *uiParamPropsEdit;
	QTreeWidgetItem *currentEditedUiItem;
	QListWidgetItem *currentEditedSensorsItem;
	FakeDevice device;
};

#endif // MAINWINDOW_H
