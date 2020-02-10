#ifndef COMMANDREACTIONCONFIGDIALOG_H
#define COMMANDREACTIONCONFIGDIALOG_H

#include <QDialog>
#include "TestDeviceConfig.h"

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
	void onEditStateChangeBeforeAnswerClicked();
	void onEditStateChangeAfterAnswerClicked();

private:
	void clearRadioBtns();

private:
	Ui::CommandReactionConfigDialog *ui;
	DeviceStateMap stateChangeBeforeAnswer;
	DeviceStateMap stateChangeAfterAnswer;
};

#endif // COMMANDREACTIONCONFIGDIALOG_H
