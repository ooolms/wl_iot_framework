#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "VDIL/xml/BlocksXmlParserFactory.h"
#include "VDIL/editor/Editor.h"
#include "wliot/client/ServerInstance.h"

namespace Ui
{
	class MainWindow;
}

class MainWindow
	:public QMainWindow
	,public WLIOTVDIL::IEditorHelper
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent=nullptr);
	~MainWindow();
	WLIOTVDIL::Editor* editor();
	virtual QString deviceName(const QUuid &devId)override;
	bool selectDevice(QUuid &deviceId,QString &deviceName,WLIOT::ControlsGroup &controls)override;
	bool selectStorage(WLIOT::StorageId &storId,QString &deviceName,WLIOT::SensorDef::Type &valuesType)override;

private slots:
	void onSaveTriggered();
	void onLoadTriggered();
	void onNewTriggered();

private:
	Ui::MainWindow *ui;
	WLIOTClient::ServerInstance srv;
	WLIOTVDIL::Editor *mEditor;
	WLIOTVDIL::Program *prg;
	WLIOTVDIL::BlocksFactory bf;
	WLIOTVDIL::BlocksXmlParserFactory xbf;
	WLIOTVDIL::BlocksEditingFactory bef;
	QMap<QUuid,QString> knownDevs;
};

#endif // MAINWINDOW_H
