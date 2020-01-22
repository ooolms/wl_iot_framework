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
#include "wliot/devices/ControlsDefinition.h"
#include "wliot/ControlUi.h"
#include "FakeDeviceBackend.h"

class MainWindow
	:public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent=nullptr);

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
	QTreeWidgetItem* mkUiGroupItem(QTreeWidgetItem *parent,const ControlsGroup &g=ControlsGroup());
	QTreeWidgetItem* mkUiControlItem(QTreeWidgetItem *parent,const CommandControl &c=CommandControl());
	QTreeWidgetItem* mkUiParamItem(QTreeWidgetItem *parent,const ControlParam &p=ControlParam());
	void dumpUiGroup(QTreeWidgetItem *item,ControlsGroup &g);
	void dumpUiCommand(QTreeWidgetItem *item,CommandControl &c);
	void dumpSensors(QList<SensorDef> &sensors);
	void saveCurrentEditedUiItem();
	void saveCurrentEditedSensorsItem();
	void rebuildControlUi();
	void buildSensorsList(const QList<SensorDef> &sensors);
	QByteArray toCVar(const QByteArray &varName, QByteArray data);

private:
	Ui::MainWindow ui;
	ElementSettingsWidget *uiParamPropsEdit;
	QTreeWidgetItem *currentEditedUiItem;
	QListWidgetItem *currentEditedSensorsItem;
	RealDevice device;
	FakeDeviceBackend *deviceBackend;
};

#endif // MAINWINDOW_H
