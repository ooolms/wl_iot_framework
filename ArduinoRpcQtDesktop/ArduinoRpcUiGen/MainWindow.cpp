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
#include "ARpcBase/ARpcSensorDef.h"
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

Q_DECLARE_METATYPE(QSharedPointer<ARpcControlsElement>)

MainWindow::MainWindow(QWidget *parent)
	:QMainWindow(parent)
{
	ui.setupUi(this);
	uiParamPropsEdit=new ElementSettingsWidget(ui.elemPropsWidget);
	(new QVBoxLayout(ui.elemPropsWidget))->addWidget(uiParamPropsEdit);
	currentEditedUiItem=0;
	currentEditedSensorsItem=0;
	ui.sensorValTypeSelect->addItem("float 32-bit",(int)ARpcSensorDef::F32);
	ui.sensorValTypeSelect->addItem("float 64-bit",(int)ARpcSensorDef::F64);
	ui.sensorValTypeSelect->addItem("signed int 8-bit",(int)ARpcSensorDef::S8);
	ui.sensorValTypeSelect->addItem("unsigned int 8-bit",(int)ARpcSensorDef::U8);
	ui.sensorValTypeSelect->addItem("signed int 16-bit",(int)ARpcSensorDef::S16);
	ui.sensorValTypeSelect->addItem("unsigned int 16-bit",(int)ARpcSensorDef::U16);
	ui.sensorValTypeSelect->addItem("signed int 32-bit",(int)ARpcSensorDef::S32);
	ui.sensorValTypeSelect->addItem("unsigned int 32-bit",(int)ARpcSensorDef::U32);
	ui.sensorValTypeSelect->addItem("signed int 64-bit",(int)ARpcSensorDef::S64);
	ui.sensorValTypeSelect->addItem("unsigned int 64-bit",(int)ARpcSensorDef::U64);
	ui.sensorValTypeSelect->addItem("text",(int)ARpcSensorDef::TEXT);

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
	QSharedPointer<ARpcControlsElement> data=item->data(0,roleValue).value<QSharedPointer<ARpcControlsElement>>();
	int type=item->data(0,roleItemType).toInt();
	currentEditedUiItem=item;
	if(type==itemTypeGroup)
		uiParamPropsEdit->editGroup((ARpcControlsGroup*)data.data());
	else if(type==itemTypeControl)
		uiParamPropsEdit->editControl((ARpcCommandControl*)data.data());
	else if(type==itemTypeParam)
		uiParamPropsEdit->editParam((ARpcControlParam*)data.data());
}

void MainWindow::onSensorsTreeSelChanged()
{
	saveCurrentEditedSensorsItem();
	currentEditedSensorsItem=0;
	if(!ui.sensorsTree->selectedItems().contains(ui.sensorsTree->currentItem()))return;
	QListWidgetItem *item=ui.sensorsTree->currentItem();
	currentEditedSensorsItem=item;
	ARpcSensorDef::Type type=item->data(roleSensorType).value<ARpcSensorDef::Type>();
	ui.sensorNoTimeBtn->setChecked(false);
	ui.sensorLocalTimeBtn->setChecked(false);
	ui.sensorGlobalTimeBtn->setChecked(false);
	ui.sensorSingleValueBtn->setChecked(false);
	ui.sensorPacketValueBtn->setChecked(false);
	if(type.packType==ARpcSensorDef::SINGLE)
		ui.sensorSingleValueBtn->setChecked(true);
	else ui.sensorPacketValueBtn->setChecked(true);
	if(type.tsType==ARpcSensorDef::NO_TIME)
		ui.sensorNoTimeBtn->setChecked(true);
	else if(type.tsType==ARpcSensorDef::LOCAL_TIME)
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
	ARpcControlsGroup grp;
	dumpUiGroup(ui.controlsTree->topLevelItem(0),grp);
	ARpcControlsGroup::dumpToXml(data,grp);
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
	ARpcControlsGroup grp;
	dumpUiGroup(ui.controlsTree->topLevelItem(0),grp);
	ARpcControlsGroup::dumpToJson(data,grp);
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
	ARpcControlsGroup grp;
	if(!ARpcControlsGroup::parseXmlDescription(data,grp,true))
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
	ARpcControlsGroup grp;
	if(!ARpcControlsGroup::parseJsonDescription(data,grp,true))
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
	ARpcControlsGroup grp;
	dumpUiGroup(ui.controlsTree->topLevelItem(0),grp);
	ARpcControlsGroup::dumpToXml(data,grp);
	data.replace('\"',"\\\"");
	qApp->clipboard()->setText("const char *interfaceStr=\""+QString::fromUtf8(data)+"\";\n");
}

void MainWindow::onCopyUiJsonAsVarTriggered()
{
	QByteArray data;
	ARpcControlsGroup grp;
	dumpUiGroup(ui.controlsTree->topLevelItem(0),grp);
	ARpcControlsGroup::dumpToJson(data,grp);
	data.replace('\"',"\\\"");
	qApp->clipboard()->setText("const char *interfaceStr=\""+QString::fromUtf8(data)+"\";\n");
}

void MainWindow::onSaveSensorsAsXmlTriggered()
{
	saveCurrentEditedSensorsItem();
	QString fileName=QFileDialog::getSaveFileName(this,tr("Save as xml"),QString(),"All files (*.*)");
	if(fileName.isEmpty())return;
	QList<ARpcSensorDef> sensors;
	dumpSensors(sensors);
	QByteArray data;
	ARpcSensorDef::dumpToXml(data,sensors);
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
	QList<ARpcSensorDef> sensors;
	dumpSensors(sensors);
	QByteArray data;
	ARpcSensorDef::dumpToJson(data,sensors);
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
	QList<ARpcSensorDef> sensors;
	if(!ARpcSensorDef::parseXmlDescription(data,sensors))
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
	QList<ARpcSensorDef> sensors;
	if(!ARpcSensorDef::parseJsonDescription(data,sensors))
	{
		QMessageBox::warning(this,tr("Error!"),tr("Parsing error"));
		return;
	}
	buildSensorsList(sensors);
}

void MainWindow::onCopySensorsXmlAsVarTriggered()
{
	QByteArray data;
	QList<ARpcSensorDef> sensors;
	dumpSensors(sensors);
	ARpcSensorDef::dumpToXml(data,sensors);
	data.replace('\"',"\\\"");
	qApp->clipboard()->setText("const char *sensorsStr=\""+QString::fromUtf8(data)+"\";\n");
}

void MainWindow::onCopySensorsJsonAsVarTriggered()
{
	QByteArray data;
	QList<ARpcSensorDef> sensors;
	dumpSensors(sensors);
	ARpcSensorDef::dumpToJson(data,sensors);
	data.replace('\"',"\\\"");
	qApp->clipboard()->setText("const char *sensorsStr=\""+QString::fromUtf8(data)+"\";\n");
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
	ARpcSensorDef::Type t;
	t.numType=ARpcSensorDef::F32;
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

QTreeWidgetItem* MainWindow::mkUiGroupItem(QTreeWidgetItem *parent,const ARpcControlsGroup &g)
{
	QTreeWidgetItem *item=new QTreeWidgetItem(parent);
	QFont f=item->font(0);
	f.setBold(true);
	item->setFont(0,f);
	item->setFlags(item->flags()|Qt::ItemIsEditable);
	item->setData(0,roleItemType,itemTypeGroup);
	ARpcControlsGroup *grp=new ARpcControlsGroup;
	*grp=g;
	QSharedPointer<ARpcControlsElement> ptr(grp);
	QVariant var;
	var.setValue<QSharedPointer<ARpcControlsElement>>(ptr);
	item->setData(0,roleValue,var);
	if(g.title.isEmpty())item->setText(0,"group");
	else item->setText(0,g.title);

	for(int i=0;i<g.elements.count();++i)
	{
		if(g.elements[i].isGroup())
			mkUiGroupItem(item,*g.elements[i].group());
		else if(g.elements[i].isControl())
			mkUiControlItem(item,*g.elements[i].control());
	}

	return item;
}

QTreeWidgetItem *MainWindow::mkUiControlItem(QTreeWidgetItem *parent,const ARpcCommandControl &c)
{
	QTreeWidgetItem *item=new QTreeWidgetItem(parent);
	QFont f=item->font(0);
	f.setBold(true);
	f.setItalic(true);
	item->setFont(0,f);
	item->setFlags(item->flags()|Qt::ItemIsEditable);
	item->setData(0,roleItemType,itemTypeControl);
	ARpcCommandControl *cmd=new ARpcCommandControl;
	*cmd=c;
	QSharedPointer<ARpcControlsElement> ptr(cmd);
	QVariant var;
	var.setValue<QSharedPointer<ARpcControlsElement>>(ptr);
	item->setData(0,roleValue,var);
	if(c.title.isEmpty())item->setText(0,"control");
	else item->setText(0,c.title);

	for(int i=0;i<c.params.count();++i)
		mkUiParamItem(item,c.params[i]);

	return item;
}

QTreeWidgetItem *MainWindow::mkUiParamItem(QTreeWidgetItem *parent,const ARpcControlParam &p)
{
	QTreeWidgetItem *item=new QTreeWidgetItem(parent);
	QFont f=item->font(0);
	item->setFont(0,f);
	item->setFlags(item->flags()|Qt::ItemIsEditable);
	item->setData(0,roleItemType,itemTypeParam);
	ARpcControlParam *param=new ARpcControlParam;
	*param=p;
	QSharedPointer<ARpcControlsElement> ptr(param);
	QVariant var;
	var.setValue<QSharedPointer<ARpcControlsElement>>(ptr);
	item->setData(0,roleValue,var);
	if(p.title.isEmpty())item->setText(0,"parameter");
	else item->setText(0,p.title);
	return item;
}

void MainWindow::dumpUiGroup(QTreeWidgetItem *item,ARpcControlsGroup &g)
{
	QSharedPointer<ARpcControlsElement> data=item->data(0,roleValue).value<QSharedPointer<ARpcControlsElement>>();
	g=*((ARpcControlsGroup*)data.data());
	g.title=item->text(0).toUtf8();
	g.elements.clear();
	for(int i=0;i<item->childCount();++i)
	{
		QTreeWidgetItem *child=item->child(i);
		int type=child->data(0,roleItemType).toInt();
		if(type==itemTypeGroup)
		{
			ARpcControlsGroup gg;
			dumpUiGroup(child,gg);
			g.elements.append(ARpcControlsGroup::Element(gg));
		}
		else if(type==itemTypeControl)
		{
			ARpcCommandControl cc;
			dumpUiCommand(child,cc);
			g.elements.append(ARpcControlsGroup::Element(cc));
		}
	}
}

void MainWindow::dumpUiCommand(QTreeWidgetItem *item, ARpcCommandControl &c)
{
	QSharedPointer<ARpcControlsElement> data=item->data(0,roleValue).value<QSharedPointer<ARpcControlsElement>>();
	c=*((ARpcCommandControl*)data.data());
	c.title=item->text(0).toUtf8();
	c.params.clear();
	for(int i=0;i<item->childCount();++i)
	{
		QTreeWidgetItem *child=item->child(i);
		int type=child->data(0,roleItemType).toInt();
		if(type!=itemTypeParam)continue;//o_O
		ARpcControlParam p;
		QSharedPointer<ARpcControlsElement> pData=child->data(0,roleValue).value<QSharedPointer<ARpcControlsElement>>();
		p=*((ARpcControlParam*)pData.data());
		p.title=child->text(0).toUtf8();
		c.params.append(p);
	}
}

void MainWindow::dumpSensors(QList<ARpcSensorDef> &sensors)
{
	sensors.clear();
	for(int i=0;i<ui.sensorsTree->count();++i)
	{
		QListWidgetItem *item=ui.sensorsTree->item(i);
		if(item->text().isEmpty())continue;
		ARpcSensorDef s;
		s.attributes=item->data(roleSensorAttributes).value<decltype(s.attributes)>();
		s.type=item->data(roleSensorType).value<ARpcSensorDef::Type>();
		s.name=item->text().toUtf8();
		sensors.append(s);
	}
}

void MainWindow::saveCurrentEditedUiItem()
{
	if(!currentEditedUiItem)return;
	QSharedPointer<ARpcControlsElement> data=
		currentEditedUiItem->data(0,roleValue).value<QSharedPointer<ARpcControlsElement>>();
	int type=currentEditedUiItem->data(0,roleItemType).toInt();
	if(type==itemTypeGroup)
		uiParamPropsEdit->saveGroup((ARpcControlsGroup*)data.data());
	else if(type==itemTypeControl)
		uiParamPropsEdit->saveControl((ARpcCommandControl*)data.data());
	else if(type==itemTypeParam)
		uiParamPropsEdit->saveParam((ARpcControlParam*)data.data());
}

void MainWindow::saveCurrentEditedSensorsItem()
{
	if(!currentEditedSensorsItem)return;
	ARpcSensorDef::Type t;
	if(ui.sensorNoTimeBtn->isChecked())
		t.tsType=ARpcSensorDef::NO_TIME;
	else if(ui.sensorLocalTimeBtn->isChecked())
		t.tsType=ARpcSensorDef::LOCAL_TIME;
	else t.tsType=ARpcSensorDef::GLOBAL_TIME;
	if(ui.sensorPacketValueBtn->isChecked())
		t.packType=ARpcSensorDef::PACKET;
	else t.packType=ARpcSensorDef::SINGLE;
	t.numType=(ARpcSensorDef::NumType)ui.sensorValTypeSelect->currentData().toInt();
	QVariantMap attributes;
	if(ui.sensorDimsEdit->value()!=1)
		attributes["dims"]=ui.sensorDimsEdit->value();
	currentEditedSensorsItem->setData(roleSensorType,QVariant::fromValue(t));
	currentEditedSensorsItem->setData(roleSensorAttributes,attributes);
}

void MainWindow::rebuildControlUi()
{
	ARpcControlsGroup grp;
	dumpUiGroup(ui.controlsTree->topLevelItem(0),grp);
	ARpcControlUi *controlUi=new ARpcControlUi(&device,grp,ui.controlsView);
	ui.controlsView->setWidget(controlUi);
	controlUi->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
}

void MainWindow::buildSensorsList(const QList<ARpcSensorDef> &sensors)
{
	currentEditedSensorsItem=0;
	ui.sensorsTree->clear();
	for(const ARpcSensorDef &s:sensors)
	{
		QListWidgetItem *item=new QListWidgetItem(ui.sensorsTree);
		item->setText(QString::fromUtf8(s.name));
		item->setData(roleSensorType,QVariant::fromValue(s.type));
		item->setData(roleSensorAttributes,QVariant::fromValue(s.attributes));
	}
}
