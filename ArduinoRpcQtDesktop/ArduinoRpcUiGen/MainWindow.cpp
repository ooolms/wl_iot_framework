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
#include "ARpcBase/ARpcSensor.h"
#include <QMessageBox>
#include <QSharedPointer>
#include <QFileDialog>
#include <QFile>
#include <QDebug>
#include <QClipboard>

static const int roleItemType=Qt::UserRole;
static const int roleValue=Qt::UserRole+1;

static const int roleSensorType=Qt::UserRole;
static const int roleSensorConstraints=Qt::UserRole+1;

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
	ARpcSensor::Type type=(ARpcSensor::Type)item->data(roleSensorType).toInt();
	QVariantMap constraints=item->data(roleSensorConstraints).toMap();
	for(QRadioButton *btn:ui.sensorTypeGroup->findChildren<QRadioButton*>())
		btn->setChecked(false);
	if(type==ARpcSensor::SINGLE)
		ui.singleNTSensorBtn->setChecked(true);
	else if(type==ARpcSensor::SINGLE_LT)
		ui.singleLTSensorBtn->setChecked(true);
	else if(type==ARpcSensor::SINGLE_GT)
		ui.singleGTSensorBtn->setChecked(true);
	else if(type==ARpcSensor::PACKET)
		ui.packetNTSensorBtn->setChecked(true);
	else if(type==ARpcSensor::PACKET_LT)
		ui.packetLTSensorBtn->setChecked(true);
	else if(type==ARpcSensor::PACKET_GT)
		ui.packetGTSensorBtn->setChecked(true);
	else ui.textSensorBtn->setChecked(true);
	if(constraints.contains("dims"))
		ui.sensorDimsEdit->setValue(constraints["dims"].toInt());
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
	if(!ARpcControlsGroup::parseXmlDescription(data,grp))
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
	if(!ARpcControlsGroup::parseJsonDescription(data,grp))
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
	QList<ARpcSensor> sensors;
	dumpSensors(sensors);
	QByteArray data;
	ARpcSensor::dumpToXml(data,sensors);
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
	QList<ARpcSensor> sensors;
	dumpSensors(sensors);
	QByteArray data;
	ARpcSensor::dumpToJson(data,sensors);
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
	QList<ARpcSensor> sensors;
	if(!ARpcSensor::parseXmlDescription(data,sensors))
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
	QList<ARpcSensor> sensors;
	if(!ARpcSensor::parseJsonDescription(data,sensors))
	{
		QMessageBox::warning(this,tr("Error!"),tr("Parsing error"));
		return;
	}
	buildSensorsList(sensors);
}

void MainWindow::onCopySensorsXmlAsVarTriggered()
{
	QByteArray data;
	QList<ARpcSensor> sensors;
	dumpSensors(sensors);
	ARpcSensor::dumpToXml(data,sensors);
	data.replace('\"',"\\\"");
	qApp->clipboard()->setText("const char *sensorsStr=\""+QString::fromUtf8(data)+"\";\n");
}

void MainWindow::onCopySensorsJsonAsVarTriggered()
{
	QByteArray data;
	QList<ARpcSensor> sensors;
	dumpSensors(sensors);
	ARpcSensor::dumpToJson(data,sensors);
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
	item->setData(roleSensorType,(int)ARpcSensor::SINGLE);
	item->setData(roleSensorConstraints,QVariantMap());
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
			ARpcControlsGroup *gg=new ARpcControlsGroup;
			dumpUiGroup(child,*gg);
			g.elements.append(ARpcControlsGroup::Element(gg));
		}
		else if(type==itemTypeControl)
		{
			ARpcCommandControl *cc=new ARpcCommandControl;
			dumpUiCommand(child,*cc);
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

void MainWindow::dumpSensors(QList<ARpcSensor> &sensors)
{
	sensors.clear();
	for(int i=0;i<ui.sensorsTree->count();++i)
	{
		QListWidgetItem *item=ui.sensorsTree->item(i);
		if(item->text().isEmpty())continue;
		ARpcSensor s;
		s.constraints=item->data(roleSensorConstraints).value<decltype(s.constraints)>();
		s.type=(ARpcSensor::Type)item->data(roleSensorType).toInt();
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
	ARpcSensor::Type t;
	if(ui.singleNTSensorBtn->isChecked())
		t=ARpcSensor::SINGLE;
	else if(ui.singleLTSensorBtn->isChecked())
		t=ARpcSensor::SINGLE_LT;
	else if(ui.singleGTSensorBtn->isChecked())
		t=ARpcSensor::SINGLE_GT;
	else if(ui.packetNTSensorBtn->isChecked())
		t=ARpcSensor::PACKET;
	else if(ui.packetLTSensorBtn->isChecked())
		t=ARpcSensor::PACKET_LT;
	else if(ui.packetGTSensorBtn->isChecked())
		t=ARpcSensor::PACKET_GT;
	else t=ARpcSensor::TEXT;
	QVariantMap constraints;
	if(ui.sensorDimsEdit->value()!=1)
		constraints["dims"]=ui.sensorDimsEdit->value();
	currentEditedSensorsItem->setData(roleSensorType,(int)t);
	currentEditedSensorsItem->setData(roleSensorConstraints,constraints);
}

void MainWindow::rebuildControlUi()
{
	ARpcControlsGroup grp;
	dumpUiGroup(ui.controlsTree->topLevelItem(0),grp);
	ARpcControlUi *controlUi=new ARpcControlUi(&device,grp,ui.controlsView);
	ui.controlsView->setWidget(controlUi);
	controlUi->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
}

void MainWindow::buildSensorsList(const QList<ARpcSensor> &sensors)
{
	currentEditedSensorsItem=0;
	ui.sensorsTree->clear();
	for(const ARpcSensor &s:sensors)
	{
		QListWidgetItem *item=new QListWidgetItem(ui.sensorsTree);
		item->setText(QString::fromUtf8(s.name));
		item->setData(roleSensorType,(int)s.type);
		item->setData(roleSensorConstraints,QVariant::fromValue(s.constraints));
	}
}
