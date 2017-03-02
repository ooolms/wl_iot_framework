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
	void onTreeSelChanged();
	void onAddGroupClicked();
	void onAddControlClicked();
	void onAddParamClicked();
	void onDelElementClicked();
	void onSaveAsJsonTriggered();
	void onSaveAsXmlTriggered();
	void onOpenXmlTriggered();
	void onOpenJsonTriggered();
	void onCopyXmlAsVarTriggered();
	void onCopyJsonAsVarTriggered();
	void onLogMsg(const QString &msg);
	void onTreeItemEdited();

private:
	QTreeWidgetItem* getCurrentGroup();
	QTreeWidgetItem* getCurrentControl();
	QTreeWidgetItem* getCurrentParam();
	QTreeWidgetItem* mkGroupItem(QTreeWidgetItem *parent,const ARpcControlsGroup &g=ARpcControlsGroup());
	QTreeWidgetItem* mkControlItem(QTreeWidgetItem *parent,const ARpcCommandControl &c=ARpcCommandControl());
	QTreeWidgetItem* mkParamItem(QTreeWidgetItem *parent,const ARpcControlParam &p=ARpcControlParam());
	void dumpGroup(QTreeWidgetItem *item,ARpcControlsGroup &g);
	void dumpCommand(QTreeWidgetItem *item,ARpcCommandControl &c);
	void saveCurrentEditedItem();
	void rebuildControlUi();

private:
	Ui::MainWindow ui;
	ElementSettingsWidget *paramPropsEdit;
	QTreeWidgetItem *currentEditedItem;
	FakeDevice device;
};

#endif // MAINWINDOW_H
