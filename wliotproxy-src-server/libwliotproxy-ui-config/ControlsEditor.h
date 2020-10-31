#ifndef CONTROLSEDITOR_H
#define CONTROLSEDITOR_H

#include <QWidget>
#include "ElementSettingsWidget.h"
#include "wliot/devices/ControlsDefinition.h"
#include "wliot/ControlUi.h"
#include "FakeDeviceBackend.h"

namespace Ui
{
	class ControlsEditor;
}

namespace WLIOTUi
{
	class ControlsEditor
		:public QWidget
	{
		Q_OBJECT
	public:
		explicit ControlsEditor(QWidget *parent=nullptr);
		virtual ~ControlsEditor();
		void setControls(const WLIOT::ControlsGroup &controls);
		WLIOT::ControlsGroup controls();

	private slots:
		void onUiTreeSelChanged();
		void onUiTreeItemEdited();
		void onAddGroupClicked();
		void onAddCommandClicked();
		void onAddCommandParamClicked();
		void onRemoveElementClicked();
		void onLogMsg(const QString &msg);

	private:
		void renderTree();
		QTreeWidgetItem* getCurrentUiGroup();
		QTreeWidgetItem* getCurrentUiControl();
		QTreeWidgetItem* getCurrentUiParam();
		QTreeWidgetItem* mkUiGroupItem(QTreeWidgetItem *parent,const WLIOT::ControlsGroup &g=WLIOT::ControlsGroup());
		QTreeWidgetItem* mkUiControlItem(QTreeWidgetItem *parent,
			const WLIOT::ControlsCommand &c=WLIOT::ControlsCommand());
		QTreeWidgetItem* mkUiParamItem(QTreeWidgetItem *parent,
			const WLIOT::ControlsCommandParam &p=WLIOT::ControlsCommandParam());
		void dumpUiGroup(QTreeWidgetItem *item,WLIOT::ControlsGroup &g);
		void dumpUiCommand(QTreeWidgetItem *item,WLIOT::ControlsCommand &c);
		void saveCurrentEditedUiItem();
		void rebuildControlUi();

	private:
		WLIOT::ControlsGroup mControls;
		QTreeWidget *controlsTree;
		ElementSettingsWidget *uiParamPropsEdit;
		Ui::ControlsEditor *ui;
		QTreeWidgetItem *currentEditedUiItem;
		WLIOT::RealDevice device;
		FakeDeviceBackend *deviceBackend;
	};
}

#endif // CONTROLSEDITOR_H
