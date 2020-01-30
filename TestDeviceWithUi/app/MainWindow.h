#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Device.h"

namespace Ui {
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

private:
	Ui::MainWindow *ui;
	Device *dev;
	QMap<QUuid,QPair<QHostAddress,QByteArray>> foundServers;
};

#endif // MAINWINDOW_H
