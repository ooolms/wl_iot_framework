#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "TestDevice.h"
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
	void loadDev(const QString &path);
	void enableDev(bool en);
	void connectToServer(const QString &hostStr);

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
	void onAnswerSyncMsgsClicked();
	void onDisconnectOnSyncTimeoutClicked();
	void onMinLogLevelSelected();
	void onEditDevStartupStateClicked();

private:
	void placeCommandReactions();
	void setRedApplyBtn(bool on);

public:
	TestDevice *dev;

private:
	Ui::MainWindow *ui;
	LogManager *log;
	ManualShowingLayoutItem *setupLayItem;
	QMap<QUuid,QPair<QHostAddress,QByteArray>> foundServers;
	QMap<QByteArray,CommandReactionConfig> cmdReactions;
	QPalette applyBtnDef,applyBtnRed;
	DeviceStateMap startupState;
};

#endif // MAINWINDOW_H
