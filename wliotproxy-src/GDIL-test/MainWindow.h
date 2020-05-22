#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "GDIL/xml/BlocksXmlParserFactory.h"
#include "GDIL/editor/Editor.h"
#include "wliot/client/ServerInstance.h"

namespace Ui
{
	class MainWindow;
}

class MainWindow
	:public QMainWindow
	,public WLIOTGDIL::IEditorHelper
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent=nullptr);
	~MainWindow();
	WLIOTGDIL::Editor* editor();
	virtual QString deviceName(const QUuid &devId)override;
	bool selectDevice(QUuid &deviceId,QString &deviceName,WLIOT::ControlsGroup &controls)override;
	bool selectStorage(WLIOT::StorageId &storId,QString &deviceName,WLIOT::SensorDef::Type &valuesType)override;

private slots:
	void onSaveTriggered();
	void onLoadTriggered();

private:
	Ui::MainWindow *ui;
	WLIOTClient::ServerInstance srv;
	WLIOTGDIL::Editor *mEditor;
	WLIOTGDIL::Program *prg;
	WLIOTGDIL::BlocksFactory bf;
	WLIOTGDIL::BlocksXmlParserFactory xbf;
	WLIOTGDIL::BlocksEditingFactory bef;
};

#endif // MAINWINDOW_H
