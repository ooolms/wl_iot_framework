#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "GDIL/xml/BlocksXmlParserFactory.h"
#include "GDIL/editor/Editor.h"
#include "IotServer.h"

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
	Editor* editor();

private slots:
	void onSaveTriggered();
	void onLoadTriggered();
	void onSelectDevice(QUuid &deviceId,QString &deviceName);
	void onSelectStorage(StorageId &storId,QString &deviceName,SensorDef::Type &valuesType);

private:
	Ui::MainWindow *ui;
	IotServer srv;
	Editor *mEditor;
	BlocksFactory bf;
	BlocksXmlParserFactory xbf;
	BlocksEditingFactory bef;
};

#endif // MAINWINDOW_H
