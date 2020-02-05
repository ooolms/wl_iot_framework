#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Device.h"
#include "ManualShowingLayoutItem.h"
#include "LogManager.h"

namespace Ui
{
	class MainWindow;
}

class MainWindow
	:public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent=nullptr);
	~MainWindow();

private slots:
	void onDeviceStateChanged();
	void onEnableDevBtnClicked();
	void onNewSrvFound(const QHostAddress &addr,const QUuid &uid,const QByteArray &name);
	void onFoundServerSelected(int index);
	void onApplyBtnClicked();
	void onSaveBtnClicked();
	void onLoadBtnClicked();
	void onHideSetupClicked();
	void onAddCmdReactionClicked();
	void onDelCmdReactionClicked();
	void onEditCmdReactionClicked();
	void onSomethingEdited();
	void onSendSensorValueClicked();
	void onConnectClicked();
	void onDisconnectClicked();

private:
	void placeCommandReactions();
	void setRedApplyBtn(bool on);

private:
	Ui::MainWindow *ui;
	LogManager *log;
	ManualShowingLayoutItem *setupLayItem;
	Device *dev;
	QMap<QUuid,QPair<QHostAddress,QByteArray>> foundServers;
	QMap<QByteArray,CommandReactionConfig> cmdReactions;
	QPalette applyBtnDef,applyBtnRed;
};

#endif // MAINWINDOW_H
