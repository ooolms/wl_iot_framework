#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "DeviceConfig.h"
#include "CommandReactionConfigDialog.h"
#include <QFileDialog>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
	:QMainWindow(parent)
	,ui(new Ui::MainWindow)
{
	dev=new Device(this);
	ui->setupUi(this);
	log=new LogManager(ui->logView,this);
	QVBoxLayout *lay=(QVBoxLayout*)ui->contentWidget->layout();
	lay->removeWidget(ui->setupWidget);
	setupLayItem=new ManualShowingLayoutItem(ui->setupWidget);
	lay->insertItem(4,setupLayItem);
	setupLayItem->setVisible(true);

	ui->minLogLevelSelect->addItem("DEBUG",(int)LogManager::DEBUG);
	ui->minLogLevelSelect->addItem("INFO",(int)LogManager::INFO);
	ui->minLogLevelSelect->addItem("WARNING",(int)LogManager::WARNING);
	ui->minLogLevelSelect->addItem("ERROR",(int)LogManager::ERROR);
	ui->minLogLevelSelect->setCurrentIndex(0);

	onDeviceStateChanged();
	connect(ui->enableDeviceBtn,&QPushButton::clicked,this,&MainWindow::onEnableDevBtnClicked);
	connect(dev,&Device::workingChanged,this,&MainWindow::onDeviceStateChanged);
	connect(dev,&Device::connected,this,&MainWindow::onDeviceStateChanged);
	connect(dev,&Device::disconnected,this,&MainWindow::onDeviceStateChanged);
	connect(dev,&Device::serverFound,this,&MainWindow::onNewSrvFound);
	connect(dev,&Device::dbgMessage,log,&LogManager::onDbgMessage);
	connect(dev,&Device::infoMessage,log,&LogManager::onInfoMessage);
	connect(dev,&Device::warningMessage,log,&LogManager::onWarningMessage);
	connect(dev,&Device::errorMessage,log,&LogManager::onErrorMessage);
	connect(ui->applyBtn,&QPushButton::clicked,this,&MainWindow::onApplyBtnClicked);
	connect(ui->saveBtn,&QPushButton::clicked,this,&MainWindow::onSaveBtnClicked);
	connect(ui->loadBtn,&QPushButton::clicked,this,&MainWindow::onLoadBtnClicked);
	connect(ui->hideSetupBtn,&QPushButton::clicked,this,&MainWindow::onHideSetupClicked);
	connect(ui->sendSensorValueBtn,&QPushButton::clicked,this,&MainWindow::onSendSensorValueClicked);
	connect(ui->addCmdReactionBtn,&QPushButton::clicked,this,&MainWindow::onAddCmdReactionClicked);
	connect(ui->delCmdReactionBtn,&QPushButton::clicked,this,&MainWindow::onDelCmdReactionClicked);
	connect(ui->editCmdReacionBtn,&QPushButton::clicked,this,&MainWindow::onEditCmdReactionClicked);
	connect(ui->connectBtn,&QPushButton::clicked,this,&MainWindow::onConnectClicked);
	connect(ui->disconnectBtn,&QPushButton::clicked,this,&MainWindow::onDisconnectClicked);
	connect(ui->detectedSrvSelect,static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
		this,&MainWindow::onFoundServerSelected);
	connect(ui->minLogLevelSelect,static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
		this,&MainWindow::onMinLogLevelSelected);
	connect(ui->answerSyncMsgsBtn,&QCheckBox::clicked,this,&MainWindow::onAnswerSyncMsgsClicked);
	connect(ui->disconnOnSyncTimeoutBtn,&QCheckBox::clicked,this,&MainWindow::onDisconnectOnSyncTimeoutClicked);
	ui->cmdReactionsList->setHeaderLabels(QStringList()<<"command"<<"reaction");
	applyBtnDef=applyBtnRed=ui->applyBtn->palette();
	applyBtnRed.setColor(QPalette::ButtonText,Qt::red);
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
	log->onInfoMessage("new server found: "+uid.toByteArray()+", "+addr.toString().toUtf8()+", "+name);
	foundServers[uid]=qMakePair(addr,name);
	ui->detectedSrvSelect->clear();
	for(auto i=foundServers.begin();i!=foundServers.end();++i)
		ui->detectedSrvSelect->addItem(i.value().first.toString()+": "+i.value().second,i.key());
}

void MainWindow::onFoundServerSelected(int index)
{
	QUuid uid=ui->detectedSrvSelect->itemData(index).toUuid();
	if(!foundServers.contains(uid))return;
	ui->addressEdit->setText(foundServers[uid].first.toString());
	dev->connectToServer(foundServers[uid].first);
}

void MainWindow::onApplyBtnClicked()
{
	QUuid uid(ui->uuidEdit->text());
	QByteArray name=ui->nameEdit->text().toUtf8();
	if(uid.isNull()||name.isEmpty())return;
	dev->setupUidAndName(uid,name);
	dev->sensorsStr=ui->sensorsEdit->toPlainText().toUtf8();
	dev->controlsStr=ui->controlsEdit->toPlainText().toUtf8();
	dev->commands=cmdReactions;
	setRedApplyBtn(false);
}

void MainWindow::onSaveBtnClicked()
{
	onApplyBtnClicked();
	QString fileName=QFileDialog::getSaveFileName(this,"","","Dev files (*.dev.xml)");
	if(fileName.isEmpty())return;
	if(!fileName.endsWith(".dev.xml"))
		fileName.append(".dev.xml");
	dev->save(fileName);
}

void MainWindow::onLoadBtnClicked()
{
	QString fileName=QFileDialog::getOpenFileName(this,"","","Dev files (*.dev.xml)");
	if(fileName.isEmpty())return;
	dev->load(fileName);
	ui->uuidEdit->setText(dev->devId.toString());
	ui->nameEdit->setText(QString::fromUtf8(dev->devName));
	ui->sensorsEdit->setPlainText(QString::fromUtf8(dev->sensorsStr));
	ui->controlsEdit->setPlainText(QString::fromUtf8(dev->controlsStr));
	cmdReactions=dev->commands;
	placeCommandReactions();
	setRedApplyBtn(false);
}

void MainWindow::onHideSetupClicked()
{
	setupLayItem->setVisible(!ui->hideSetupBtn->isChecked());
}

void MainWindow::onAddCmdReactionClicked()
{
	QByteArray cmd=QInputDialog::getText(this,"Command","command").toUtf8();
	if(cmd.isEmpty()||cmdReactions.contains(cmd))return;
	CommandReactionConfigDialog dlg(this);
	if(dlg.exec()!=QDialog::Accepted)return;
	CommandReactionConfig cfg=dlg.config();
	cmdReactions[cmd]=cfg;
	placeCommandReactions();
	setRedApplyBtn(true);
}

void MainWindow::onDelCmdReactionClicked()
{
	if(!ui->cmdReactionsList->selectedItems().contains(ui->cmdReactionsList->currentItem()))return;
	QByteArray cmd=ui->cmdReactionsList->currentItem()->text(0).toUtf8();
	if(!cmdReactions.contains(cmd))return;
	cmdReactions.remove(cmd);
	placeCommandReactions();
}

void MainWindow::onEditCmdReactionClicked()
{
	if(!ui->cmdReactionsList->selectedItems().contains(ui->cmdReactionsList->currentItem()))return;
	QByteArray cmd=ui->cmdReactionsList->currentItem()->text(0).toUtf8();
	if(!cmdReactions.contains(cmd))return;
	CommandReactionConfig &cfg=cmdReactions[cmd];
	CommandReactionConfigDialog dlg(this);
	dlg.setConfig(cfg);
	if(dlg.exec()!=QDialog::Accepted)return;
	cfg=dlg.config();
	placeCommandReactions();
	setRedApplyBtn(true);
}

void MainWindow::onSomethingEdited()
{
	setRedApplyBtn(true);
}

void MainWindow::onSendSensorValueClicked()
{
	if(ui->sensorValueEdit->text().isEmpty()||ui->sensorNameEdit->text().isEmpty())return;
	QByteArrayList args=ui->sensorValueEdit->text().toUtf8().split('|');
	dev->sendSensorValue(ui->sensorNameEdit->text().toUtf8(),args);
}

void MainWindow::onConnectClicked()
{
	if(!dev->isConnected())
		dev->connectToServer(QHostAddress(ui->addressEdit->text()));
}

void MainWindow::onDisconnectClicked()
{
	dev->disconnectFromServer();
}

void MainWindow::onAnswerSyncMsgsClicked()
{
	dev->answerSyncMsgs=ui->answerSyncMsgsBtn->isChecked();
}

void MainWindow::onDisconnectOnSyncTimeoutClicked()
{
	dev->disconnectOnSyncTimeout=ui->disconnOnSyncTimeoutBtn->isChecked();
}

void MainWindow::onMinLogLevelSelected()
{
	LogManager::Level l=(LogManager::Level)ui->minLogLevelSelect->currentData().toInt();
	log->setMinLevel(l);
}

void MainWindow::placeCommandReactions()
{
	ui->cmdReactionsList->clear();
	for(auto i=cmdReactions.begin();i!=cmdReactions.end();++i)
	{
		QTreeWidgetItem *item=new QTreeWidgetItem(ui->cmdReactionsList);
		item->setText(0,i.key());
		CommandReactionConfig &cfg=i.value();
		item->setText(1,cmdActToString(cfg.act));
		if(cfg.act==CommandReaction::CMD_ANSWER_OK||cfg.act==CommandReaction::CMD_ANSWER_ERR)
		{
			QByteArray s=Message("",cfg.retVal).dump();
			s.chop(1);
			s.remove(0,1);
			item->setToolTip(1,QString::fromUtf8(s));
		}
		else if(cfg.act==CommandReaction::CMD_JS_EXEC)
			item->setToolTip(1,cfg.jsScript);
	}
}

void MainWindow::setRedApplyBtn(bool on)
{
	if(on)
		ui->applyBtn->setPalette(applyBtnRed);
	else ui->applyBtn->setPalette(applyBtnDef);

}
