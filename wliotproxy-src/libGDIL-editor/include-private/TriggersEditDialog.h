#ifndef TRIGGERSEDITDIALOG_H
#define TRIGGERSEDITDIALOG_H

#include <QDialog>
#include "GDIL/core/Program.h"
#include "GDIL/editor/IEditorHelper.h"

class QListWidget;
class QCheckBox;

namespace Ui
{
	class TriggersEditDialog;
}

namespace WLIOTGDIL
{
	class TriggersEditDialog
		:public QDialog
	{
		Q_OBJECT
	public:
		explicit TriggersEditDialog(Program *p,IEditorHelper *hlp,QWidget *parent=0);
		~TriggersEditDialog();

	private slots:
		void onOkClicked();

	private:
		Ui::TriggersEditDialog *ui;
		Program *prg;
		QListWidget *storageTriggersList;
		QMap<QCheckBox*,WLIOT::StorageId> storageTriggersMap;
	};
}

#endif // TRIGGERSEDITDIALOG_H
