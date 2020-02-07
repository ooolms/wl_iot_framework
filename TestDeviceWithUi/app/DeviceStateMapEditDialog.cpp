#include "DeviceStateMapEditDialog.h"
#include "ui_DeviceStateMapEditDialog.h"

DeviceStateMapEditDialog::DeviceStateMapEditDialog(QWidget *parent)
	:QDialog(parent)
	,ui(new Ui::DeviceStateMapEditDialog)
{
	ui->setupUi(this);
	ui->commandsStateTree->setHeaderLabels(QStringList()<<"command / param index (0-based)"<<"value");
	ui->additionalParamsStateTree->setHeaderLabels(QStringList()<<"param name"<<"value");

	connect(ui->buttonBox,&QDialogButtonBox::accepted,this,&DeviceStateMapEditDialog::accept);
	connect(ui->buttonBox,&QDialogButtonBox::rejected,this,&DeviceStateMapEditDialog::reject);
	connect(ui->addCmdBtn,&QPushButton::clicked,this,&DeviceStateMapEditDialog::onAddCmdClicked);
	connect(ui->addCmdParamBtn,&QPushButton::clicked,this,&DeviceStateMapEditDialog::onAddCmdParamClicked);
	connect(ui->delCmdElemBtn,&QPushButton::clicked,this,&DeviceStateMapEditDialog::onDelCmdElemClicked);
	connect(ui->addAdditParamBtn,&QPushButton::clicked,this,&DeviceStateMapEditDialog::onAddAdditParamClicked);
	connect(ui->delAdditParamBtn,&QPushButton::clicked,this,&DeviceStateMapEditDialog::onDelAdditParamClicked);
}

DeviceStateMapEditDialog::~DeviceStateMapEditDialog()
{
	delete ui;
}

void DeviceStateMapEditDialog::setMap(const DeviceStateMap &map)
{
	ui->commandsStateTree->clear();
	for(auto i=map.commands.begin();i!=map.commands.end();++i)
	{
		auto &paramMap=i.value();
		QTreeWidgetItem *cmdItem=new QTreeWidgetItem(ui->commandsStateTree);
		cmdItem->setText(0,QString::fromUtf8(i.key()));
		cmdItem->setFlags(cmdItem->flags()|Qt::ItemIsEditable);
		for(auto j=paramMap.begin();j!=paramMap.end();++j)
		{
			QTreeWidgetItem *paramItem=new QTreeWidgetItem(cmdItem);
			paramItem->setText(0,QString::number(j.key()));
			paramItem->setText(1,QString::fromUtf8(j.value()));
			paramItem->setFlags(paramItem->flags()|Qt::ItemIsEditable);
		}
	}

	ui->additionalParamsStateTree->clear();
	for(auto i=map.additionalParams.begin();i!=map.additionalParams.end();++i)
	{
		QTreeWidgetItem *item=new QTreeWidgetItem(ui->additionalParamsStateTree);
		item->setText(0,QString::fromUtf8(i.key()));
		item->setText(1,QString::fromUtf8(i.value()));
		item->setFlags(item->flags()|Qt::ItemIsEditable);
	}
}

DeviceStateMap DeviceStateMapEditDialog::getMap()
{
	DeviceStateMap map;
	for(int i=0;i<ui->commandsStateTree->topLevelItemCount();++i)
	{
		QTreeWidgetItem *cmdItem=ui->commandsStateTree->topLevelItem(i);
		if(cmdItem->text(0).isEmpty())continue;
		auto &paramsMap=map.commands[cmdItem->text(0).toUtf8()];
		for(int j=0;j<cmdItem->childCount();++j)
		{
			QTreeWidgetItem *paramItem=cmdItem->child(j);
			bool ok=false;
			quint32 paramIndex=paramItem->text(0).toUInt(&ok);
			if(!ok)continue;
			paramsMap[paramIndex]=paramItem->text(1).toUtf8();
		}
	}
	for(int i=0;i<ui->additionalParamsStateTree->topLevelItemCount();++i)
	{
		QTreeWidgetItem *item=ui->additionalParamsStateTree->topLevelItem(i);
		if(item->text(0).isEmpty())continue;
		map.additionalParams[item->text(0).toUtf8()]=item->text(1).toUtf8();
	}
	return map;
}

void DeviceStateMapEditDialog::resizeEvent(QResizeEvent *ev)
{
	QDialog::resizeEvent(ev);
	ui->commandsStateTree->setColumnWidth(0,(ui->commandsStateTree->width()-20)/2);
	ui->additionalParamsStateTree->setColumnWidth(0,(ui->additionalParamsStateTree->width()-20)/2);
}

void DeviceStateMapEditDialog::onAddCmdClicked()
{
	QTreeWidgetItem *item=new QTreeWidgetItem(ui->commandsStateTree);
	item->setFlags(item->flags()|Qt::ItemIsEditable);
}

void DeviceStateMapEditDialog::onAddCmdParamClicked()
{
	QTreeWidgetItem *item=ui->commandsStateTree->currentItem();
	if(item->parent())return;
	QTreeWidgetItem *paramItem=new QTreeWidgetItem(item);
	paramItem->setFlags(paramItem->flags()|Qt::ItemIsEditable);
}

void DeviceStateMapEditDialog::onDelCmdElemClicked()
{
	if(ui->commandsStateTree->currentItem())
		delete ui->commandsStateTree->currentItem();
}

void DeviceStateMapEditDialog::onAddAdditParamClicked()
{
	QTreeWidgetItem *item=new QTreeWidgetItem(ui->additionalParamsStateTree);
	item->setFlags(item->flags()|Qt::ItemIsEditable);
}

void DeviceStateMapEditDialog::onDelAdditParamClicked()
{
	if(ui->additionalParamsStateTree->currentItem())
		delete ui->additionalParamsStateTree->currentItem();
}
