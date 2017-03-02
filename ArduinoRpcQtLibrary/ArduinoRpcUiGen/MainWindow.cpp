#include "MainWindow.h"
#include <QMessageBox>
#include <QSharedPointer>
#include <QFileDialog>
#include <QFile>
#include <QDebug>
#include <QClipboard>

static const int roleItemType=Qt::UserRole;
static const int roleValue=Qt::UserRole+1;

static const int itemTypeGroup=1;
static const int itemTypeControl=2;
static const int itemTypeParam=3;

Q_DECLARE_METATYPE(QSharedPointer<ARpcControlsElement>)

MainWindow::MainWindow(QWidget *parent)
	:QMainWindow(parent)
{
	ui.setupUi(this);
	paramPropsEdit=new ElementSettingsWidget(ui.elemPropsWidget);
	(new QVBoxLayout(ui.elemPropsWidget))->addWidget(paramPropsEdit);
	currentEditedItem=0;

	connect(ui.controlsTree,&QTreeWidget::itemSelectionChanged,this,&MainWindow::onTreeSelChanged,Qt::QueuedConnection);
//	connect(ui.controlsTree,&QTreeWidget::itemChanged,this,&MainWindow::onTreeItemEdited);
	connect(ui.addGroupBtn,&QPushButton::clicked,this,&MainWindow::onAddGroupClicked);
	connect(ui.addControlBtn,&QPushButton::clicked,this,&MainWindow::onAddControlClicked);
	connect(ui.addParamBtn,&QPushButton::clicked,this,&MainWindow::onAddParamClicked);
	connect(ui.removeBtn,&QPushButton::clicked,this,&MainWindow::onDelElementClicked);
	connect(ui.saveXmlAction,&QAction::triggered,this,&MainWindow::onSaveAsXmlTriggered);
	connect(ui.saveJsonAction,&QAction::triggered,this,&MainWindow::onSaveAsJsonTriggered);
	connect(ui.openXmlAction,&QAction::triggered,this,&MainWindow::onOpenXmlTriggered);
	connect(ui.openJsonAction,&QAction::triggered,this,&MainWindow::onOpenJsonTriggered);
	connect(ui.copyXmlAsCVarAction,&QAction::triggered,this,&MainWindow::onCopyXmlAsVarTriggered);
	connect(ui.copyJsonAsCVarAction,&QAction::triggered,this,&MainWindow::onCopyJsonAsVarTriggered);
	connect(&device,&FakeDevice::logMsg,this,&MainWindow::onLogMsg);

	QTreeWidgetItem *item=mkGroupItem(ui.controlsTree->invisibleRootItem());
	item->setText(0,"Device controls");
	ui.controlsTree->clearSelection();
	item->setSelected(true);
	ui.controlsTree->setCurrentItem(item);
}

void MainWindow::onTreeSelChanged()
{
	saveCurrentEditedItem();
	currentEditedItem=0;
	rebuildControlUi();
	if(!ui.controlsTree->selectedItems().contains(ui.controlsTree->currentItem()))return;
	QTreeWidgetItem *item=ui.controlsTree->currentItem();
	QSharedPointer<ARpcControlsElement> data=item->data(0,roleValue).value<QSharedPointer<ARpcControlsElement>>();
	int type=item->data(0,roleItemType).toInt();
	currentEditedItem=item;
	if(type==itemTypeGroup)
		paramPropsEdit->editGroup((ARpcControlsGroup*)data.data());
	else if(type==itemTypeControl)
		paramPropsEdit->editControl((ARpcCommandControl*)data.data());
	else if(type==itemTypeParam)
		paramPropsEdit->editParam((ARpcControlParam*)data.data());
}

void MainWindow::onAddGroupClicked()
{
	if(!ui.controlsTree->selectedItems().contains(ui.controlsTree->currentItem()))return;
	if(ui.controlsTree->currentItem()->data(0,roleItemType)!=itemTypeGroup)return;
	QTreeWidgetItem *item=mkGroupItem(ui.controlsTree->currentItem());
	ui.controlsTree->editItem(item);
	ui.controlsTree->clearSelection();
	item->setSelected(true);
	ui.controlsTree->setCurrentItem(item);
}

void MainWindow::onAddControlClicked()
{
	if(!ui.controlsTree->selectedItems().contains(ui.controlsTree->currentItem()))return;
	if(ui.controlsTree->currentItem()->data(0,roleItemType)!=itemTypeGroup)return;
	QTreeWidgetItem *item=mkControlItem(ui.controlsTree->currentItem());
	ui.controlsTree->clearSelection();
	item->setSelected(true);
	ui.controlsTree->setCurrentItem(item);
	ui.controlsTree->editItem(item);
}

void MainWindow::onAddParamClicked()
{
	if(!ui.controlsTree->selectedItems().contains(ui.controlsTree->currentItem()))return;
	if(ui.controlsTree->currentItem()->data(0,roleItemType)!=itemTypeControl)return;
	QTreeWidgetItem *item=mkParamItem(ui.controlsTree->currentItem());
	ui.controlsTree->clearSelection();
	item->setSelected(true);
	ui.controlsTree->setCurrentItem(item);
	ui.controlsTree->editItem(item);
}

void MainWindow::onDelElementClicked()
{
	if(!ui.controlsTree->selectedItems().contains(ui.controlsTree->currentItem()))return;
	if(ui.controlsTree->currentItem()->parent()==0)return;
	if(QMessageBox::question(this,tr("Sure?"),tr("Remove element?"))!=QMessageBox::Yes)return;
	currentEditedItem=0;
	delete ui.controlsTree->currentItem();
}

void MainWindow::onSaveAsJsonTriggered()
{
	saveCurrentEditedItem();
	QString fileName=QFileDialog::getSaveFileName(this,tr("Save as json"),QString(),"All files (*.*)");
	if(fileName.isEmpty())return;
	saveCurrentEditedItem();
	QString data;
	ARpcControlsGroup grp;
	dumpGroup(ui.controlsTree->topLevelItem(0),grp);
	ARpcControlsGroup::dumpToJson(data,grp);
	QFile file(fileName);
	if(!file.open(QIODevice::WriteOnly))return;
	file.write(data.toUtf8());
	file.close();
}

void MainWindow::onSaveAsXmlTriggered()
{
	QString fileName=QFileDialog::getSaveFileName(this,tr("Save as xml"),QString(),"All files (*.*)");
	if(fileName.isEmpty())return;
	saveCurrentEditedItem();
	QString data;
	ARpcControlsGroup grp;
	dumpGroup(ui.controlsTree->topLevelItem(0),grp);
	ARpcControlsGroup::dumpToXml(data,grp);
	QFile file(fileName);
	if(!file.open(QIODevice::WriteOnly))return;
	file.write(data.toUtf8());
	file.close();
}

void MainWindow::onOpenXmlTriggered()
{
	QString fileName=QFileDialog::getOpenFileName(this,tr("Open xml"),QString(),"All files (*.*)");
	if(fileName.isEmpty())return;
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly))return;
	QString data=QString::fromUtf8(file.readAll());
	file.close();
	ARpcControlsGroup grp;
	if(!ARpcControlsGroup::parseXmlDescription(data,grp))
	{
		QMessageBox::warning(this,tr("Error!"),tr("Parsing error"));
		return;
	}
	currentEditedItem=0;
	ui.controlsTree->clear();
	mkGroupItem(ui.controlsTree->invisibleRootItem(),grp);
}

void MainWindow::onOpenJsonTriggered()
{
	QString fileName=QFileDialog::getOpenFileName(this,tr("Open json"),QString(),"All files (*.*)");
	if(fileName.isEmpty())return;
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly))return;
	QString data=QString::fromUtf8(file.readAll());
	file.close();
	ARpcControlsGroup grp;
	if(!ARpcControlsGroup::parseJsonDescription(data,grp))
	{
		QMessageBox::warning(this,tr("Error!"),tr("Parsing error"));
		return;
	}
	currentEditedItem=0;
	ui.controlsTree->clear();
	mkGroupItem(ui.controlsTree->invisibleRootItem(),grp);
}

void MainWindow::onCopyXmlAsVarTriggered()
{
	QString data;
	ARpcControlsGroup grp;
	dumpGroup(ui.controlsTree->topLevelItem(0),grp);
	ARpcControlsGroup::dumpToXml(data,grp);
	data.replace('\"',"\\\"");
	qApp->clipboard()->setText("const char *interfaceStr=\""+data+"\";\n");
}

void MainWindow::onCopyJsonAsVarTriggered()
{
	QString data;
	ARpcControlsGroup grp;
	dumpGroup(ui.controlsTree->topLevelItem(0),grp);
	ARpcControlsGroup::dumpToJson(data,grp);
	data.replace('\"',"\\\"");
	qApp->clipboard()->setText("const char *interfaceStr=\""+data+"\";\n");
}

void MainWindow::onLogMsg(const QString &msg)
{
	QTextCursor cur=ui.commandsLogView->textCursor();
	cur.movePosition(QTextCursor::End);
	cur.insertText(msg);
	cur.insertBlock();
}

void MainWindow::onTreeItemEdited()
{
	rebuildControlUi();
}

QTreeWidgetItem *MainWindow::getCurrentGroup()
{
	if(!ui.controlsTree->selectedItems().contains(ui.controlsTree->currentItem()))return 0;
	if(ui.controlsTree->currentItem()->data(0,roleItemType)!=itemTypeGroup)return 0;
	return ui.controlsTree->currentItem();
}

QTreeWidgetItem *MainWindow::getCurrentControl()
{
	if(!ui.controlsTree->selectedItems().contains(ui.controlsTree->currentItem()))return 0;
	if(ui.controlsTree->currentItem()->data(0,roleItemType)!=itemTypeControl)return 0;
	return ui.controlsTree->currentItem();
}

QTreeWidgetItem *MainWindow::getCurrentParam()
{
	if(!ui.controlsTree->selectedItems().contains(ui.controlsTree->currentItem()))return 0;
	if(ui.controlsTree->currentItem()->data(0,roleItemType)!=itemTypeParam)return 0;
	return ui.controlsTree->currentItem();
}

QTreeWidgetItem* MainWindow::mkGroupItem(QTreeWidgetItem *parent,const ARpcControlsGroup &g)
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
			mkGroupItem(item,*g.elements[i].group());
		else if(g.elements[i].isControl())
			mkControlItem(item,*g.elements[i].control());
	}

	return item;
}

QTreeWidgetItem *MainWindow::mkControlItem(QTreeWidgetItem *parent,const ARpcCommandControl &c)
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
		mkParamItem(item,c.params[i]);

	return item;
}

QTreeWidgetItem *MainWindow::mkParamItem(QTreeWidgetItem *parent,const ARpcControlParam &p)
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

void MainWindow::dumpGroup(QTreeWidgetItem *item,ARpcControlsGroup &g)
{
	QSharedPointer<ARpcControlsElement> data=item->data(0,roleValue).value<QSharedPointer<ARpcControlsElement>>();
	g=*((ARpcControlsGroup*)data.data());
	g.title=item->text(0);
	g.elements.clear();
	for(int i=0;i<item->childCount();++i)
	{
		QTreeWidgetItem *child=item->child(i);
		int type=child->data(0,roleItemType).toInt();
		if(type==itemTypeGroup)
		{
			ARpcControlsGroup *gg=new ARpcControlsGroup;
			dumpGroup(child,*gg);
			g.elements.append(ARpcControlsGroup::Element(gg));
		}
		else if(type==itemTypeControl)
		{
			ARpcCommandControl *cc=new ARpcCommandControl;
			dumpCommand(child,*cc);
			g.elements.append(ARpcControlsGroup::Element(cc));
		}
	}
}

void MainWindow::dumpCommand(QTreeWidgetItem *item, ARpcCommandControl &c)
{
	QSharedPointer<ARpcControlsElement> data=item->data(0,roleValue).value<QSharedPointer<ARpcControlsElement>>();
	c=*((ARpcCommandControl*)data.data());
	c.title=item->text(0);
	c.params.clear();
	for(int i=0;i<item->childCount();++i)
	{
		QTreeWidgetItem *child=item->child(i);
		int type=child->data(0,roleItemType).toInt();
		if(type!=itemTypeParam)continue;//o_O
		ARpcControlParam p;
		QSharedPointer<ARpcControlsElement> pData=child->data(0,roleValue).value<QSharedPointer<ARpcControlsElement>>();
		p=*((ARpcControlParam*)pData.data());
		p.title=child->text(0);
		c.params.append(p);
	}
}

void MainWindow::saveCurrentEditedItem()
{
	if(!currentEditedItem)return;
	QSharedPointer<ARpcControlsElement> data=
		currentEditedItem->data(0,roleValue).value<QSharedPointer<ARpcControlsElement>>();
	int type=currentEditedItem->data(0,roleItemType).toInt();
	if(type==itemTypeGroup)
		paramPropsEdit->saveGroup((ARpcControlsGroup*)data.data());
	else if(type==itemTypeControl)
		paramPropsEdit->saveControl((ARpcCommandControl*)data.data());
	else if(type==itemTypeParam)
		paramPropsEdit->saveParam((ARpcControlParam*)data.data());
}

void MainWindow::rebuildControlUi()
{
	ARpcControlsGroup grp;
	dumpGroup(ui.controlsTree->topLevelItem(0),grp);
	ARpcControlUi *controlUi=new ARpcControlUi(&device,grp,ui.controlsView);
	ui.controlsView->setWidget(controlUi);
	controlUi->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
}
