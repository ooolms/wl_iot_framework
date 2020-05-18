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
	mEditor=new Editor(&bf,&xbf,&bef,static_cast<IEditorHelper*>(this),this);

	setCentralWidget(mEditor);
	connect(ui->saveAction,&QAction::triggered,this,&MainWindow::onSaveTriggered);
	connect(ui->loadAction,&QAction::triggered,this,&MainWindow::onLoadTriggered);
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

QString MainWindow::deviceName(const QUuid &devId)
{
	if(srv.connection()->isConnected())
		return srv.findDevName(devId);
	return QString();
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

bool MainWindow::selectDevice(QUuid &deviceId,QString &deviceName,ControlsGroup &controls)
{
	if(!srv.connection()->isConnected())
	{
		ui->statusbar->showMessage("server disconnected",5000);
		return false;
	}

	QDialog dlg;
	QListWidget *list=new QListWidget(&dlg);
	QDialogButtonBox *btns=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,&dlg);
	connect(btns,&QDialogButtonBox::accepted,&dlg,&QDialog::accept);
	connect(btns,&QDialogButtonBox::rejected,&dlg,&QDialog::reject);
	QVBoxLayout *lay=new QVBoxLayout(&dlg);
	lay->addWidget(list,1);
	lay->addWidget(btns);

	QSet<QUuid> addedDevs;

	for(QUuid uid:srv.devices()->identifiedDevices())
	{
		RealDevice *dev=srv.devices()->devById(uid);
		if(!dev)continue;
		QListWidgetItem *item=new QListWidgetItem(list);
		item->setText(QString::fromUtf8(dev->name()));
		item->setData(Qt::UserRole,dev->id());
		QFont f=item->font();
		f.setBold(true);
		item->setFont(f);
		addedDevs.insert(dev->id());
	}

	for(StorageId id:srv.storages()->allStorages())
	{
		ISensorStorage *st=srv.storages()->existingStorage(id);
		if(!st)continue;
		if(addedDevs.contains(st->deviceId()))continue;
		QListWidgetItem *item=new QListWidgetItem(list);
		item->setText(QString::fromUtf8(st->deviceName()));
		item->setData(Qt::UserRole,st->deviceId());
	}

	if(dlg.exec()!=QDialog::Accepted)
		return false;
	if(list->selectedItems().isEmpty())
		return false;
	QListWidgetItem *item=list->selectedItems()[0];
	QUuid id=item->data(Qt::UserRole).toUuid();
	RealDevice *dev=srv.devices()->devById(id);
	if(dev)
		dev->getControlsDescription(controls);
	else controls=ControlsGroup();
	deviceId=id;
	deviceName=item->text();
	return true;
}

bool MainWindow::selectStorage(StorageId &storId,QString &deviceName,SensorDef::Type &valuesType)
{
	if(!srv.connection()->isConnected())
	{
		ui->statusbar->showMessage("server disconnected",5000);
		return false;
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
		return false;
	if(!list->selectedItems().contains(list->currentItem()))
		return false;
	int index=list->currentRow();
	storId=ids[index];
	ISensorStorage *st=srv.storages()->existingStorage(storId);
	deviceName=st->deviceName();
	valuesType=st->storedValuesType();
	return true;
}
