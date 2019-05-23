/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "MainWindow.h"
#include "wliot/devices/SensorDef.h"
#include <QMessageBox>
#include <QSharedPointer>
#include <QFileDialog>
#include <QFile>
#include <QDebug>
#include <QClipboard>

static const int roleItemType=Qt::UserRole;
static const int roleValue=Qt::UserRole+1;

static const int roleSensorType=Qt::UserRole;
static const int roleSensorAttributes=Qt::UserRole+1;

static const int itemTypeGroup=1;
static const int itemTypeControl=2;
static const int itemTypeParam=3;

Q_DECLARE_METATYPE(QSharedPointer<ControlsElement>)

MainWindow::MainWindow(QWidget *parent)
	:QMainWindow(parent)
{
	ui.setupUi(this);
	uiParamPropsEdit=new ElementSettingsWidget(ui.elemPropsWidget);
	(new QVBoxLayout(ui.elemPropsWidget))->addWidget(uiParamPropsEdit);
	currentEditedUiItem=0;
	currentEditedSensorsItem=0;
	ui.sensorValTypeSelect->addItem("float 32-bit",(int)SensorDef::F32);
	ui.sensorValTypeSelect->addItem("float 64-bit",(int)SensorDef::F64);
	ui.sensorValTypeSelect->addItem("signed int 8-bit",(int)SensorDef::S8);
	ui.sensorValTypeSelect->addItem("unsigned int 8-bit",(int)SensorDef::U8);
	ui.sensorValTypeSelect->addItem("signed int 16-bit",(int)SensorDef::S16);
	ui.sensorValTypeSelect->addItem("unsigned int 16-bit",(int)SensorDef::U16);
	ui.sensorValTypeSelect->addItem("signed int 32-bit",(int)SensorDef::S32);
	ui.sensorValTypeSelect->addItem("unsigned int 32-bit",(int)SensorDef::U32);
	ui.sensorValTypeSelect->addItem("signed int 64-bit",(int)SensorDef::S64);
	ui.sensorValTypeSelect->addItem("unsigned int 64-bit",(int)SensorDef::U64);
	ui.sensorValTypeSelect->addItem("text",(int)SensorDef::TEXT);

	connect(ui.controlsTree,&QTreeWidget::itemSelectionChanged,this,&MainWindow::onUiTreeSelChanged,
		Qt::QueuedConnection);
//	connect(ui.controlsTree,&QTreeWidget::itemChanged,this,&MainWindow::onUiTreeItemEdited);
	//TODO !!!
	connect(ui.sensorsTree,&QListWidget::itemSelectionChanged,this,&MainWindow::onSensorsTreeSelChanged,
		Qt::QueuedConnection);
	connect(ui.addGroupBtn,&QPushButton::clicked,this,&MainWindow::onAddUiGroupClicked);
	connect(ui.addControlBtn,&QPushButton::clicked,this,&MainWindow::onAddUiControlClicked);
	connect(ui.addParamBtn,&QPushButton::clicked,this,&MainWindow::onAddUiParamClicked);
	connect(ui.removeBtn,&QPushButton::clicked,this,&MainWindow::onDelUiElementClicked);
	connect(ui.saveUiXmlAction,&QAction::triggered,this,&MainWindow::onSaveUiAsXmlTriggered);
	connect(ui.saveUiJsonAction,&QAction::triggered,this,&MainWindow::onSaveUiAsJsonTriggered);
	connect(ui.openUiXmlAction,&QAction::triggered,this,&MainWindow::onOpenUiXmlTriggered);
	connect(ui.openUiJsonAction,&QAction::triggered,this,&MainWindow::onOpenUiJsonTriggered);
	connect(ui.copyUiXmlAsCVarAction,&QAction::triggered,this,&MainWindow::onCopyUiXmlAsVarTriggered);
	connect(ui.copyUiJsonAsCVarAction,&QAction::triggered,this,&MainWindow::onCopyUiJsonAsVarTriggered);
	connect(ui.saveSensorsXmlAction,&QAction::triggered,this,&MainWindow::onSaveSensorsAsXmlTriggered);
	connect(ui.saveSensorsJsonAction,&QAction::triggered,this,&MainWindow::onSaveSensorsAsJsonTriggered);
	connect(ui.openSensorsXmlAction,&QAction::triggered,this,&MainWindow::onOpenSensorsXmlTriggered);
	connect(ui.openSensorsJsonAction,&QAction::triggered,this,&MainWindow::onOpenSensorsJsonTriggered);
	connect(ui.copySensorsXmlAsCVarAction,&QAction::triggered,this,&MainWindow::onCopySensorsXmlAsVarTriggered);
	connect(ui.copySensorsJsonAsCVarAction,&QAction::triggered,this,&MainWindow::onCopySensorsJsonAsVarTriggered);
	connect(ui.addSensorBtn,&QPushButton::clicked,this,&MainWindow::onAddSensorClicked);
	connect(ui.delSensorBtn,&QPushButton::clicked,this,&MainWindow::onDelSensorClicked);
	connect(ui.uuidGenBtn,&QPushButton::clicked,this,&MainWindow::onGenUuidTriggered);
	connect(&device,&FakeDevice::logMsg,this,&MainWindow::onLogMsg);

	QTreeWidgetItem *item=mkUiGroupItem(ui.controlsTree->invisibleRootItem());
	item->setText(0,"Device controls");
	ui.controlsTree->clearSelection();
	item->setSelected(true);
	ui.controlsTree->setCurrentItem(item);
}

void MainWindow::onUiTreeSelChanged()
{
	saveCurrentEditedUiItem();
	currentEditedUiItem=0;
	rebuildControlUi();
	if(!ui.controlsTree->selectedItems().contains(ui.controlsTree->currentItem()))return;
	QTreeWidgetItem *item=ui.controlsTree->currentItem();
	QSharedPointer<ControlsElement> data=item->data(0,roleValue).value<QSharedPointer<ControlsElement>>();
	int type=item->data(0,roleItemType).toInt();
	currentEditedUiItem=item;
	if(type==itemTypeGroup)
		uiParamPropsEdit->editGroup((ControlsGroup*)data.data());
	else if(type==itemTypeControl)
		uiParamPropsEdit->editControl((CommandControl*)data.data());
	else if(type==itemTypeParam)
		uiParamPropsEdit->editParam((ControlParam*)data.data());
}

void MainWindow::onSensorsTreeSelChanged()
{
	saveCurrentEditedSensorsItem();
	currentEditedSensorsItem=0;
	if(!ui.sensorsTree->selectedItems().contains(ui.sensorsTree->currentItem()))return;
	QListWidgetItem *item=ui.sensorsTree->currentItem();
	currentEditedSensorsItem=item;
	SensorDef::Type type=item->data(roleSensorType).value<SensorDef::Type>();
	ui.sensorNoTimeBtn->setChecked(false);
	ui.sensorLocalTimeBtn->setChecked(false);
	ui.sensorGlobalTimeBtn->setChecked(false);
	ui.sensorSingleValueBtn->setChecked(false);
	ui.sensorPacketValueBtn->setChecked(false);
	if(type.packType==SensorDef::SINGLE)
		ui.sensorSingleValueBtn->setChecked(true);
	else ui.sensorPacketValueBtn->setChecked(true);
	if(type.tsType==SensorDef::NO_TIME)
		ui.sensorNoTimeBtn->setChecked(true);
	else if(type.tsType==SensorDef::LOCAL_TIME)
		ui.sensorLocalTimeBtn->setChecked(true);
	else ui.sensorGlobalTimeBtn->setChecked(true);
	ui.sensorValTypeSelect->setCurrentIndex(0);
	for(int i=0;i<ui.sensorValTypeSelect->count();++i)
	{
		if(ui.sensorValTypeSelect->itemData(i).toInt()==(int)type.numType)
		{
			ui.sensorValTypeSelect->setCurrentIndex(i);
			break;
		}
	}
	QVariantMap attributes=item->data(roleSensorAttributes).toMap();
	if(attributes.contains("dims"))
		ui.sensorDimsEdit->setValue(attributes["dims"].toInt());
	else ui.sensorDimsEdit->setValue(1);
}

void MainWindow::onAddUiGroupClicked()
{
	if(!ui.controlsTree->selectedItems().contains(ui.controlsTree->currentItem()))return;
	if(ui.controlsTree->currentItem()->data(0,roleItemType)!=itemTypeGroup)return;
	QTreeWidgetItem *item=mkUiGroupItem(ui.controlsTree->currentItem());
	ui.controlsTree->editItem(item);
	ui.controlsTree->clearSelection();
	item->setSelected(true);
	ui.controlsTree->setCurrentItem(item);
}

void MainWindow::onAddUiControlClicked()
{
	if(!ui.controlsTree->selectedItems().contains(ui.controlsTree->currentItem()))return;
	if(ui.controlsTree->currentItem()->data(0,roleItemType)!=itemTypeGroup)return;
	QTreeWidgetItem *item=mkUiControlItem(ui.controlsTree->currentItem());
	ui.controlsTree->clearSelection();
	item->setSelected(true);
	ui.controlsTree->setCurrentItem(item);
	ui.controlsTree->editItem(item);
}

void MainWindow::onAddUiParamClicked()
{
	if(!ui.controlsTree->selectedItems().contains(ui.controlsTree->currentItem()))return;
	if(ui.controlsTree->currentItem()->data(0,roleItemType)!=itemTypeControl)return;
	QTreeWidgetItem *item=mkUiParamItem(ui.controlsTree->currentItem());
	ui.controlsTree->clearSelection();
	item->setSelected(true);
	ui.controlsTree->setCurrentItem(item);
	ui.controlsTree->editItem(item);
}

void MainWindow::onDelUiElementClicked()
{
	saveCurrentEditedUiItem();
	if(!ui.controlsTree->selectedItems().contains(ui.controlsTree->currentItem()))return;
	if(ui.controlsTree->currentItem()->parent()==0)return;
	if(QMessageBox::question(this,tr("Sure?"),tr("Remove element?"))!=QMessageBox::Yes)return;
	currentEditedUiItem=0;
	delete ui.controlsTree->currentItem();
}

void MainWindow::onSaveUiAsXmlTriggered()
{
	QString fileName=QFileDialog::getSaveFileName(this,tr("Save as xml"),QString(),"All files (*.*)");
	if(fileName.isEmpty())return;
	saveCurrentEditedUiItem();
	QByteArray data;
	ControlsGroup grp;
	dumpUiGroup(ui.controlsTree->topLevelItem(0),grp);
	ControlsGroup::dumpToXml(data,grp);
	QFile file(fileName);
	if(!file.open(QIODevice::WriteOnly))return;
	file.write(data);
	file.close();
}

void MainWindow::onSaveUiAsJsonTriggered()
{
	QString fileName=QFileDialog::getSaveFileName(this,tr("Save as json"),QString(),"All files (*.*)");
	if(fileName.isEmpty())return;
	saveCurrentEditedUiItem();
	QByteArray data;
	ControlsGroup grp;
	dumpUiGroup(ui.controlsTree->topLevelItem(0),grp);
	ControlsGroup::dumpToJson(data,grp);
	QFile file(fileName);
	if(!file.open(QIODevice::WriteOnly))return;
	file.write(data);
	file.close();
}

void MainWindow::onOpenUiXmlTriggered()
{
	QString fileName=QFileDialog::getOpenFileName(this,tr("Open xml"),QString(),"All files (*.*)");
	if(fileName.isEmpty())return;
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly))return;
	QByteArray data=file.readAll();
	file.close();
	ControlsGroup grp;
	if(!ControlsGroup::parseXmlDescription(data,grp,true))
	{
		QMessageBox::warning(this,tr("Error!"),tr("Parsing error"));
		return;
	}
	currentEditedUiItem=0;
	ui.controlsTree->clear();
	mkUiGroupItem(ui.controlsTree->invisibleRootItem(),grp);
}

void MainWindow::onOpenUiJsonTriggered()
{
	QString fileName=QFileDialog::getOpenFileName(this,tr("Open json"),QString(),"All files (*.*)");
	if(fileName.isEmpty())return;
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly))return;
	QByteArray data=file.readAll();
	file.close();
	ControlsGroup grp;
	if(!ControlsGroup::parseJsonDescription(data,grp,true))
	{
		QMessageBox::warning(this,tr("Error!"),tr("Parsing error"));
		return;
	}
	currentEditedUiItem=0;
	ui.controlsTree->clear();
	mkUiGroupItem(ui.controlsTree->invisibleRootItem(),grp);
}

void MainWindow::onCopyUiXmlAsVarTriggered()
{
	QByteArray data;
	ControlsGroup grp;
	dumpUiGroup(ui.controlsTree->topLevelItem(0),grp);
	ControlsGroup::dumpToXml(data,grp,true);
	qApp->clipboard()->setText(QString::fromUtf8(toCVar("interfaceStr",data)));
}

void MainWindow::onCopyUiJsonAsVarTriggered()
{
	QByteArray data;
	ControlsGroup grp;
	dumpUiGroup(ui.controlsTree->topLevelItem(0),grp);
	ControlsGroup::dumpToJson(data,grp);
	qApp->clipboard()->setText(QString::fromUtf8(toCVar("interfaceStr",data)));
}

void MainWindow::onSaveSensorsAsXmlTriggered()
{
	saveCurrentEditedSensorsItem();
	QString fileName=QFileDialog::getSaveFileName(this,tr("Save as xml"),QString(),"All files (*.*)");
	if(fileName.isEmpty())return;
	QList<SensorDef> sensors;
	dumpSensors(sensors);
	QByteArray data;
	SensorDef::dumpToXml(data,sensors);
	QFile file(fileName);
	if(!file.open(QIODevice::WriteOnly))return;
	file.write(data);
	file.close();
}

void MainWindow::onSaveSensorsAsJsonTriggered()
{
	saveCurrentEditedSensorsItem();
	QString fileName=QFileDialog::getSaveFileName(this,tr("Save as json"),QString(),"All files (*.*)");
	if(fileName.isEmpty())return;
	QList<SensorDef> sensors;
	dumpSensors(sensors);
	QByteArray data;
	SensorDef::dumpToJson(data,sensors);
	QFile file(fileName);
	if(!file.open(QIODevice::WriteOnly))return;
	file.write(data);
	file.close();
}

void MainWindow::onOpenSensorsXmlTriggered()
{
	QString fileName=QFileDialog::getOpenFileName(this,tr("Open xml"),QString(),"All files (*.*)");
	if(fileName.isEmpty())return;
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly))return;
	QByteArray data=file.readAll();
	file.close();
	QList<SensorDef> sensors;
	if(!SensorDef::parseXmlDescription(data,sensors))
	{
		QMessageBox::warning(this,tr("Error!"),tr("Parsing error"));
		return;
	}
	buildSensorsList(sensors);
}

void MainWindow::onOpenSensorsJsonTriggered()
{
	QString fileName=QFileDialog::getOpenFileName(this,tr("Open json"),QString(),"All files (*.*)");
	if(fileName.isEmpty())return;
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly))return;
	QByteArray data=file.readAll();
	file.close();
	QList<SensorDef> sensors;
	if(!SensorDef::parseJsonDescription(data,sensors))
	{
		QMessageBox::warning(this,tr("Error!"),tr("Parsing error"));
		return;
	}
	buildSensorsList(sensors);
}

void MainWindow::onCopySensorsXmlAsVarTriggered()
{
	QByteArray data;
	QList<SensorDef> sensors;
	dumpSensors(sensors);
	SensorDef::dumpToXml(data,sensors,true);
	qApp->clipboard()->setText(QString::fromUtf8(toCVar("sensorsStr",data)));
}

void MainWindow::onCopySensorsJsonAsVarTriggered()
{
	QByteArray data;
	QList<SensorDef> sensors;
	dumpSensors(sensors);
	SensorDef::dumpToJson(data,sensors);
	qApp->clipboard()->setText(QString::fromUtf8(toCVar("sensorsStr",data)));
}

void MainWindow::onLogMsg(const QString &msg)
{
	QTextCursor cur=ui.commandsLogView->textCursor();
	cur.movePosition(QTextCursor::End);
	cur.insertText(msg);
	cur.insertBlock();
}

void MainWindow::onUiTreeItemEdited()
{
	rebuildControlUi();
}

void MainWindow::onAddSensorClicked()
{
	QListWidgetItem *item=new QListWidgetItem(ui.sensorsTree);
	item->setSelected(true);
	item->setFlags(item->flags()|Qt::ItemIsEditable);
	SensorDef::Type t;
	t.numType=SensorDef::F32;
	item->setData(roleSensorType,QVariant::fromValue(t));
	item->setData(roleSensorAttributes,QVariantMap());
	ui.sensorsTree->clearSelection();
	ui.sensorsTree->editItem(item);
}

void MainWindow::onDelSensorClicked()
{
	saveCurrentEditedSensorsItem();
	if(!ui.sensorsTree->selectedItems().contains(ui.sensorsTree->currentItem()))return;
	currentEditedSensorsItem=0;
	delete ui.sensorsTree->currentItem();

}

void MainWindow::onGenUuidTriggered()
{
	QUuid id=QUuid::createUuid();
	ui.uuidViewText->setText(id.toString());
	ui.uuidCompressedHex->setText(QString::fromUtf8(id.toRfc4122().toHex()));
}

QTreeWidgetItem *MainWindow::getCurrentUiGroup()
{
	if(!ui.controlsTree->selectedItems().contains(ui.controlsTree->currentItem()))return 0;
	if(ui.controlsTree->currentItem()->data(0,roleItemType)!=itemTypeGroup)return 0;
	return ui.controlsTree->currentItem();
}

QTreeWidgetItem *MainWindow::getCurrentUiControl()
{
	if(!ui.controlsTree->selectedItems().contains(ui.controlsTree->currentItem()))return 0;
	if(ui.controlsTree->currentItem()->data(0,roleItemType)!=itemTypeControl)return 0;
	return ui.controlsTree->currentItem();
}

QTreeWidgetItem *MainWindow::getCurrentUiParam()
{
	if(!ui.controlsTree->selectedItems().contains(ui.controlsTree->currentItem()))return 0;
	if(ui.controlsTree->currentItem()->data(0,roleItemType)!=itemTypeParam)return 0;
	return ui.controlsTree->currentItem();
}

QTreeWidgetItem* MainWindow::mkUiGroupItem(QTreeWidgetItem *parent,const ControlsGroup &g)
{
	QTreeWidgetItem *item=new QTreeWidgetItem(parent);
	QFont f=item->font(0);
	f.setBold(true);
	item->setFont(0,f);
	item->setFlags(item->flags()|Qt::ItemIsEditable);
	item->setData(0,roleItemType,itemTypeGroup);
	ControlsGroup *grp=new ControlsGroup;
	*grp=g;
	QSharedPointer<ControlsElement> ptr(grp);
	QVariant var;
	var.setValue<QSharedPointer<ControlsElement>>(ptr);
	item->setData(0,roleValue,var);
	item->setText(0,g.title);

	for(int i=0;i<g.elements.count();++i)
	{
		if(g.elements[i].isGroup())
			mkUiGroupItem(item,*g.elements[i].group());
		else if(g.elements[i].isControl())
			mkUiControlItem(item,*g.elements[i].control());
	}

	return item;
}

QTreeWidgetItem *MainWindow::mkUiControlItem(QTreeWidgetItem *parent,const CommandControl &c)
{
	QTreeWidgetItem *item=new QTreeWidgetItem(parent);
	QFont f=item->font(0);
	f.setBold(true);
	f.setItalic(true);
	item->setFont(0,f);
	item->setFlags(item->flags()|Qt::ItemIsEditable);
	item->setData(0,roleItemType,itemTypeControl);
	CommandControl *cmd=new CommandControl;
	*cmd=c;
	QSharedPointer<ControlsElement> ptr(cmd);
	QVariant var;
	var.setValue<QSharedPointer<ControlsElement>>(ptr);
	item->setData(0,roleValue,var);
	item->setText(0,c.title);

	for(int i=0;i<c.params.count();++i)
		mkUiParamItem(item,c.params[i]);

	return item;
}

QTreeWidgetItem *MainWindow::mkUiParamItem(QTreeWidgetItem *parent,const ControlParam &p)
{
	QTreeWidgetItem *item=new QTreeWidgetItem(parent);
	QFont f=item->font(0);
	item->setFont(0,f);
	item->setFlags(item->flags()|Qt::ItemIsEditable);
	item->setData(0,roleItemType,itemTypeParam);
	ControlParam *param=new ControlParam;
	*param=p;
	QSharedPointer<ControlsElement> ptr(param);
	QVariant var;
	var.setValue<QSharedPointer<ControlsElement>>(ptr);
	item->setData(0,roleValue,var);
	item->setText(0,p.title);
	return item;
}

void MainWindow::dumpUiGroup(QTreeWidgetItem *item,ControlsGroup &g)
{
	QSharedPointer<ControlsElement> data=item->data(0,roleValue).value<QSharedPointer<ControlsElement>>();
	g=*((ControlsGroup*)data.data());
	g.title=item->text(0).toUtf8();
	g.elements.clear();
	for(int i=0;i<item->childCount();++i)
	{
		QTreeWidgetItem *child=item->child(i);
		int type=child->data(0,roleItemType).toInt();
		if(type==itemTypeGroup)
		{
			ControlsGroup gg;
			dumpUiGroup(child,gg);
			g.elements.append(ControlsGroup::Element(gg));
		}
		else if(type==itemTypeControl)
		{
			CommandControl cc;
			dumpUiCommand(child,cc);
			g.elements.append(ControlsGroup::Element(cc));
		}
	}
}

void MainWindow::dumpUiCommand(QTreeWidgetItem *item, CommandControl &c)
{
	QSharedPointer<ControlsElement> data=item->data(0,roleValue).value<QSharedPointer<ControlsElement>>();
	c=*((CommandControl*)data.data());
	c.title=item->text(0).toUtf8();
	c.params.clear();
	for(int i=0;i<item->childCount();++i)
	{
		QTreeWidgetItem *child=item->child(i);
		int type=child->data(0,roleItemType).toInt();
		if(type!=itemTypeParam)continue;//o_O
		ControlParam p;
		QSharedPointer<ControlsElement> pData=child->data(0,roleValue).value<QSharedPointer<ControlsElement>>();
		p=*((ControlParam*)pData.data());
		p.title=child->text(0).toUtf8();
		c.params.append(p);
	}
}

void MainWindow::dumpSensors(QList<SensorDef> &sensors)
{
	sensors.clear();
	for(int i=0;i<ui.sensorsTree->count();++i)
	{
		QListWidgetItem *item=ui.sensorsTree->item(i);
		if(item->text().isEmpty())continue;
		SensorDef s;
		s.attributes=item->data(roleSensorAttributes).value<decltype(s.attributes)>();
		s.type=item->data(roleSensorType).value<SensorDef::Type>();
		s.name=item->text().toUtf8();
		sensors.append(s);
	}
}

void MainWindow::saveCurrentEditedUiItem()
{
	if(!currentEditedUiItem)return;
	QSharedPointer<ControlsElement> data=
		currentEditedUiItem->data(0,roleValue).value<QSharedPointer<ControlsElement>>();
	int type=currentEditedUiItem->data(0,roleItemType).toInt();
	if(type==itemTypeGroup)
		uiParamPropsEdit->saveGroup((ControlsGroup*)data.data());
	else if(type==itemTypeControl)
		uiParamPropsEdit->saveControl((CommandControl*)data.data());
	else if(type==itemTypeParam)
		uiParamPropsEdit->saveParam((ControlParam*)data.data());
}

void MainWindow::saveCurrentEditedSensorsItem()
{
	if(!currentEditedSensorsItem)return;
	SensorDef::Type t;
	if(ui.sensorNoTimeBtn->isChecked())
		t.tsType=SensorDef::NO_TIME;
	else if(ui.sensorLocalTimeBtn->isChecked())
		t.tsType=SensorDef::LOCAL_TIME;
	else t.tsType=SensorDef::GLOBAL_TIME;
	if(ui.sensorPacketValueBtn->isChecked())
		t.packType=SensorDef::PACKET;
	else t.packType=SensorDef::SINGLE;
	t.numType=(SensorDef::NumType)ui.sensorValTypeSelect->currentData().toInt();
	QVariantMap attributes;
	if(ui.sensorDimsEdit->value()!=1)
		attributes["dims"]=ui.sensorDimsEdit->value();
	currentEditedSensorsItem->setData(roleSensorType,QVariant::fromValue(t));
	currentEditedSensorsItem->setData(roleSensorAttributes,attributes);
}

void MainWindow::rebuildControlUi()
{
	ControlsGroup grp;
	dumpUiGroup(ui.controlsTree->topLevelItem(0),grp);
	ControlUi *controlUi=new ControlUi(&device,grp,ui.controlsView);
	ui.controlsView->setWidget(controlUi);
	controlUi->adjustSize();
}

void MainWindow::buildSensorsList(const QList<SensorDef> &sensors)
{
	currentEditedSensorsItem=0;
	ui.sensorsTree->clear();
	for(const SensorDef &s:sensors)
	{
		QListWidgetItem *item=new QListWidgetItem(ui.sensorsTree);
		item->setText(QString::fromUtf8(s.name));
		item->setData(roleSensorType,QVariant::fromValue(s.type));
		item->setData(roleSensorAttributes,QVariant::fromValue(s.attributes));
	}
}

QByteArray MainWindow::toCVar(const QByteArray &varName,QByteArray data)
{
	data.replace('\"',"\\\"");
	QByteArray varData="const char *"+varName+"=";
	QByteArrayList tmp=data.split('\n');
	for(const QByteArray &str:tmp)
		if(!str.isEmpty())
			varData.append("\n\""+str+"\"");
	varData.append(";\n");
	return varData;
}
