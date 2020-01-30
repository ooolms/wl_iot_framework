#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
	:QMainWindow(parent)
	,ui(new Ui::MainWindow)
{
	dev=new Device(this);
	ui->setupUi(this);
	onDeviceStateChanged();
	connect(ui->enableDeviceBtn,&QPushButton::clicked,this,&MainWindow::onEnableDevBtnClicked);
	connect(dev,&Device::workingChanged,this,&MainWindow::onDeviceStateChanged);
	connect(dev,&Device::connected,this,&MainWindow::onDeviceStateChanged);
	connect(dev,&Device::disconnected,this,&MainWindow::onDeviceStateChanged);
	connect(dev,&Device::serverFound,this,&MainWindow::onNewSrvFound);
	connect(ui->detectedSrvSelect,static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
		this,&MainWindow::onFoundServerSelected);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::onDeviceStateChanged()
{
	QString stateStr;
	if(dev->isWorking())
		stateStr+="enabled";
	else stateStr+="disabled";
	stateStr+=", ";
	if(dev->isConnected())
	{
		stateStr+="connected, ";
		stateStr+=dev->serverAddr().toString();
	}
	else stateStr+="disconnected";
	ui->setupWidget->setEnabled(!dev->isWorking());
	ui->sendSensorValueWidget->setEnabled(dev->isWorking());
	ui->statusLabel->setText(stateStr);
	ui->enableDeviceBtn->setChecked(dev->isWorking());
}

void MainWindow::onEnableDevBtnClicked()
{
	dev->setWorking(!dev->isWorking());
}

void MainWindow::onNewSrvFound(const QHostAddress &addr,const QUuid &uid,const QByteArray &name)
{
	if(foundServers.contains(uid))
	{
		auto &v=foundServers[uid];
		if(v.first==addr&&v.second==name)
			return;
	}
	foundServers[uid]=qMakePair(addr,name);
	ui->detectedSrvSelect->clear();
	for(auto i=foundServers.begin();i!=foundServers.end();++i)
		ui->detectedSrvSelect->addItem(i.value().first.toString()+": "+i.value().second,i.key());
}

void MainWindow::onFoundServerSelected(int index)
{
	QUuid uid=ui->detectedSrvSelect->itemData(index).toUuid();
	if(!foundServers.contains(uid))return;
	dev->connectToServer(foundServers[uid].first);
}
