#ifndef TRIGGERSEDITDIALOG_H
#define TRIGGERSEDITDIALOG_H

#include <QDialog>
#include "VDIL/core/Program.h"
#include "VDIL/editor/IEditorHelper.h"

class QListWidget;
class QCheckBox;

namespace Ui
{
	class TriggersEditDialog;
}

namespace WLIOTVDIL
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
