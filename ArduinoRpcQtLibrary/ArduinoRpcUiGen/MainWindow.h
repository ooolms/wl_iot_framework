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

private:
	QTreeWidgetItem* getCurrentUiGroup();
	QTreeWidgetItem* getCurrentUiControl();
	QTreeWidgetItem* getCurrentUiParam();
	QTreeWidgetItem* mkUiGroupItem(QTreeWidgetItem *parent,const ARpcControlsGroup &g=ARpcControlsGroup());
	QTreeWidgetItem* mkUiControlItem(QTreeWidgetItem *parent,const ARpcCommandControl &c=ARpcCommandControl());
	QTreeWidgetItem* mkUiParamItem(QTreeWidgetItem *parent,const ARpcControlParam &p=ARpcControlParam());
	void dumpUiGroup(QTreeWidgetItem *item,ARpcControlsGroup &g);
	void dumpUiCommand(QTreeWidgetItem *item,ARpcCommandControl &c);
	void dumpSensors(QList<ARpcSensor> &sensors);
	void saveCurrentEditedUiItem();
	void saveCurrentEditedSensorsItem();
	void rebuildControlUi();
	void buildSensorsList(const QList<ARpcSensor> &sensors);

private:
	Ui::MainWindow ui;
	ElementSettingsWidget *uiParamPropsEdit;
	QTreeWidgetItem *currentEditedUiItem;
	QListWidgetItem *currentEditedSensorsItem;
	FakeDevice device;
};

#endif // MAINWINDOW_H
