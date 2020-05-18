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
	,public IEditorHelper
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent=nullptr);
	~MainWindow();
	Editor* editor();
	virtual QString deviceName(const QUuid &devId)override;
	bool selectDevice(QUuid &deviceId,QString &deviceName,ControlsGroup &controls)override;
	bool selectStorage(StorageId &storId,QString &deviceName,SensorDef::Type &valuesType)override;

private slots:
	void onSaveTriggered();
	void onLoadTriggered();

private:
	Ui::MainWindow *ui;
	IotServer srv;
	Editor *mEditor;
	Program *prg;
	BlocksFactory bf;
	BlocksXmlParserFactory xbf;
	BlocksEditingFactory bef;
};

#endif // MAINWINDOW_H
