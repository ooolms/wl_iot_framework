#ifndef COMMANDREACTIONCONFIGDIALOG_H
#define COMMANDREACTIONCONFIGDIALOG_H

#include <QDialog>
#include "DeviceConfig.h"

namespace Ui
{
	class CommandReactionConfigDialog;
}

class CommandReactionConfigDialog
	:public QDialog
{
	Q_OBJECT
public:
	explicit CommandReactionConfigDialog(QWidget *parent=nullptr);
	~CommandReactionConfigDialog();
	CommandReactionConfig config();
	void setConfig(const CommandReactionConfig &c);
	void onAddArgClicked();
	void onDelArgClicked();

private:
	void clearRadioBtns();

private:
	Ui::CommandReactionConfigDialog *ui;
};

#endif // COMMANDREACTIONCONFIGDIALOG_H
