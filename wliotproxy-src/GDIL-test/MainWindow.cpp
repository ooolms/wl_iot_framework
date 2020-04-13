#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QFileDialog>
#include <QListWidget>
#include <QDialogButtonBox>
#include <QLayout>

MainWindow::MainWindow(QWidget *parent)
	:QMainWindow(parent)
	,ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	mEditor=new Editor(&bf,&xbf,&bef,this);
	setCentralWidget(mEditor);
	connect(ui->saveAction,&QAction::triggered,this,&MainWindow::onSaveTriggered);
	connect(ui->loadAction,&QAction::triggered,this,&MainWindow::onLoadTriggered);
	connect(mEditor,&Editor::selectDevice,this,&MainWindow::onSelectDevice);
	connect(mEditor,&Editor::selectStorage,this,&MainWindow::onSelectStorage);
	if(!srv.connection()->startConnectLocal()||!srv.connection()->waitForConnected())
		ui->statusbar->showMessage("connection failed",5000);
}

MainWindow::~MainWindow()
{
	delete ui;
}

Editor* MainWindow::editor()
{
	return mEditor;
}

void MainWindow::onSaveTriggered()
{
	QString path=QFileDialog::getSaveFileName(this,"Save","","GDIL program (*.gdil)");
	if(!path.endsWith(".gdil"))
		path.append(".gdil");
	QFile file(path);
	if(!file.open(QIODevice::WriteOnly))
		return;
	file.write(mEditor->getProgram());
	file.close();
}

void MainWindow::onLoadTriggered()
{
	QString path=QFileDialog::getOpenFileName(this,"Save","","GDIL program (*.gdil)");
	QFile file(path);
	if(!file.open(QIODevice::ReadOnly))
		return;
	mEditor->setProgram(file.readAll());
	file.close();
}

void MainWindow::onSelectDevice(QUuid &deviceId,QString &deviceName)
{
	if(!srv.connection()->isConnected())
	{
		ui->statusbar->showMessage("server disconnected",5000);
		return;
	}

	QDialog dlg;
	QListWidget *list=new QListWidget(&dlg);
	QDialogButtonBox *btns=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,&dlg);
	connect(btns,&QDialogButtonBox::accepted,&dlg,&QDialog::accept);
	connect(btns,&QDialogButtonBox::rejected,&dlg,&QDialog::reject);
	QVBoxLayout *lay=new QVBoxLayout(&dlg);
	lay->addWidget(list,1);
	lay->addWidget(btns);

	QMap<QUuid,QByteArray> devMap;
	for(QUuid uid:srv.devices()->identifiedDevices())
	{
		RealDevice *dev=srv.devices()->devById(uid);
		if(!dev)continue;
		devMap[dev->id()]=dev->name();
	}
	QList<StorageId> ids;
	srv.storages()->listStorages(ids);
	for(StorageId id:ids)
	{
		ISensorStorage *st=srv.storages()->existingStorage(id);
		if(!st)continue;
		if(!devMap.contains(st->deviceId()))
			devMap[st->deviceId()]=st->deviceName();
	}

	for(auto i=devMap.begin();i!=devMap.end();++i)
	{
		QListWidgetItem *item=new QListWidgetItem(list);
		item->setText(QString::fromUtf8(i.value()));
		item->setData(Qt::UserRole,i.key());
	}

	if(dlg.exec()!=QDialog::Accepted)
		return;
	if(list->selectedItems().isEmpty())
		return;
	deviceId=list->selectedItems()[0]->data(Qt::UserRole).toUuid();
	deviceName=list->selectedItems()[0]->text();
}

void MainWindow::onSelectStorage(StorageId &storId,QString &deviceName,SensorDef::Type &valuesType)
{
	if(!srv.connection()->isConnected())
	{
		ui->statusbar->showMessage("server disconnected",5000);
		return;
	}

	QDialog dlg;
	QListWidget *list=new QListWidget(&dlg);
	QDialogButtonBox *btns=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,&dlg);
	connect(btns,&QDialogButtonBox::accepted,&dlg,&QDialog::accept);
	connect(btns,&QDialogButtonBox::rejected,&dlg,&QDialog::reject);
	QVBoxLayout *lay=new QVBoxLayout(&dlg);
	lay->addWidget(list,1);
	lay->addWidget(btns);

	QList<StorageId> ids;
	srv.storages()->listStorages(ids);
	for(StorageId id:ids)
	{
		ISensorStorage *st=srv.storages()->existingStorage(id);
		if(!st)continue;
		QListWidgetItem *item=new QListWidgetItem(list);
		item->setText(st->deviceName()+" ("+id.deviceId.toString()+"): "+id.sensorName);
	}

	if(dlg.exec()!=QDialog::Accepted)
		return;
	if(!list->selectedItems().contains(list->currentItem()))
		return;
	int index=list->currentRow();
	storId=ids[index];
	ISensorStorage *st=srv.storages()->existingStorage(storId);
	deviceName=st->deviceName();
	valuesType=st->storedValuesType();
}
