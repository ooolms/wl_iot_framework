#ifndef DEVICESTATEMAPEDITDIALOG_H
#define DEVICESTATEMAPEDITDIALOG_H

#include <QDialog>
#include "TestDeviceState.h"

namespace Ui
{
	class DeviceStateMapEditDialog;
}

class DeviceStateMapEditDialog
	:public QDialog
{
	Q_OBJECT

public:
	explicit DeviceStateMapEditDialog(QWidget *parent=nullptr);
	~DeviceStateMapEditDialog();
	void setMap(const DeviceStateMap &map);
	DeviceStateMap getMap();

protected:
	virtual void resizeEvent(QResizeEvent *ev)override;

private slots:
	void onAddCmdClicked();
	void onAddCmdParamClicked();
	void onDelCmdElemClicked();
	void onAddAdditParamClicked();
	void onDelAdditParamClicked();

private:
	Ui::DeviceStateMapEditDialog *ui;
};

#endif // DEVICESTATEMAPEDITDIALOG_H
