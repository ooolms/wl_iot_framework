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
	QMap<QCheckBox*,StorageId> storageTriggersMap;
};

#endif // TRIGGERSEDITDIALOG_H
