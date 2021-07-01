#include "ControlsEditor.h"
#include "ui_ControlsEditor.h"
#include <QMessageBox>

using namespace WLIOT;
using namespace WLIOTUi;

static const int roleItemType=Qt::UserRole;
static const int roleValue=Qt::UserRole+1;

static const int itemTypeGroup=1;
static const int itemTypeControl=2;
static const int itemTypeParam=3;

Q_DECLARE_METATYPE(QSharedPointer<ControlsElement>)

ControlsEditor::ControlsEditor(QWidget *parent)
	:QWidget(parent)
{
	deviceBackend=new FakeDeviceBackend(&device);
	device.setBackend(deviceBackend);
	device.identify();
	currentEditedUiItem=0;

	ui=new Ui::ControlsEditor;
	ui->setupUi(this);
	ui->commandsLogView->setMaximumBlockCount(1000);

	uiParamPropsEdit=new ElementSettingsWidget(ui->elemPropsWidget);
	(new QVBoxLayout(ui->elemPropsWidget))->addWidget(uiParamPropsEdit);

	connect(ui->controlsTree,&QTreeWidget::itemSelectionChanged,this,&ControlsEditor::onUiTreeSelChanged,
		Qt::QueuedConnection);
	connect(ui->addGroupBtn,&QPushButton::clicked,this,&ControlsEditor::onAddGroupClicked);
	connect(ui->addCommandBtn,&QPushButton::clicked,this,&ControlsEditor::onAddCommandClicked);
	connect(ui->addParamBtn,&QPushButton::clicked,this,&ControlsEditor::onAddCommandParamClicked);
	connect(ui->removeBtn,&QPushButton::clicked,this,&ControlsEditor::onRemoveElementClicked);
	connect(ui->clearLogBtn,&QPushButton::clicked,this,&ControlsEditor::onClearLogClicked);
	connect(deviceBackend,&FakeDeviceBackend::logMsg,this,&ControlsEditor::onLogMsg);

	QTreeWidgetItem *item=mkUiGroupItem(ui->controlsTree->invisibleRootItem());
	item->setText(0,"Device controls");
	item->setFlags(item->flags()|Qt::ItemIsEditable);
	ui->controlsTree->clearSelection();
	item->setSelected(true);
	ui->controlsTree->setCurrentItem(item);
}

ControlsEditor::~ControlsEditor()
{
	delete ui;
}

void ControlsEditor::setControls(const ControlsGroup &controls)
{
	currentEditedUiItem=0;
	ui->controlsTree->clear();
	uiParamPropsEdit->resetCurrent();
	mkUiGroupItem(ui->controlsTree->invisibleRootItem(),controls);
}

ControlsGroup ControlsEditor::controls()
{
	saveCurrentEditedUiItem();
	ControlsGroup retVal;
	dumpUiGroup(ui->controlsTree->topLevelItem(0),retVal);
	return retVal;
}

void ControlsEditor::onUiTreeSelChanged()
{
	saveCurrentEditedUiItem();
	currentEditedUiItem=0;
	rebuildControlUi();
	if(!ui->controlsTree->selectedItems().contains(ui->controlsTree->currentItem()))return;
	QTreeWidgetItem *item=ui->controlsTree->currentItem();
	QSharedPointer<ControlsElement> data=item->data(0,roleValue).value<QSharedPointer<ControlsElement>>();
	currentEditedUiItem=item;
	uiParamPropsEdit->editControl(data.data());
}

void ControlsEditor::onAddGroupClicked()
{
	if(!ui->controlsTree->selectedItems().contains(ui->controlsTree->currentItem()))return;
	if(ui->controlsTree->currentItem()->data(0,roleItemType)!=itemTypeGroup)return;
	QTreeWidgetItem *item=mkUiGroupItem(ui->controlsTree->currentItem());
	ui->controlsTree->editItem(item);
	ui->controlsTree->clearSelection();
	item->setSelected(true);
	ui->controlsTree->setCurrentItem(item);
}

void ControlsEditor::onAddCommandClicked()
{
	if(!ui->controlsTree->selectedItems().contains(ui->controlsTree->currentItem()))return;
	if(ui->controlsTree->currentItem()->data(0,roleItemType)!=itemTypeGroup)return;
	QTreeWidgetItem *item=mkUiControlItem(ui->controlsTree->currentItem());
	ui->controlsTree->clearSelection();
	item->setSelected(true);
	ui->controlsTree->setCurrentItem(item);
	ui->controlsTree->editItem(item);
}

void ControlsEditor::onAddCommandParamClicked()
{
	if(!ui->controlsTree->selectedItems().contains(ui->controlsTree->currentItem()))return;
	if(ui->controlsTree->currentItem()->data(0,roleItemType)!=itemTypeControl)return;
	QTreeWidgetItem *item=mkUiParamItem(ui->controlsTree->currentItem());
	ui->controlsTree->clearSelection();
	item->setSelected(true);
	ui->controlsTree->setCurrentItem(item);
	ui->controlsTree->editItem(item);
}

void ControlsEditor::onRemoveElementClicked()
{
	saveCurrentEditedUiItem();
	if(!ui->controlsTree->selectedItems().contains(ui->controlsTree->currentItem()))return;
	if(ui->controlsTree->currentItem()->parent()==0)return;
	if(QMessageBox::question(this,tr("Sure?"),tr("Remove element?"))!=QMessageBox::Yes)return;
	currentEditedUiItem=0;
	delete ui->controlsTree->currentItem();
	uiParamPropsEdit->resetCurrent();
}

void ControlsEditor::onClearLogClicked()
{
	ui->commandsLogView->clear();
}

void ControlsEditor::onLogMsg(const QString &msg)
{
	QTextCursor cur=ui->commandsLogView->textCursor();
	cur.movePosition(QTextCursor::End);
	cur.insertBlock();
	cur.insertText(msg);
}

void ControlsEditor::onUiTreeItemEdited()
{
	rebuildControlUi();
}

QTreeWidgetItem* ControlsEditor::getCurrentUiGroup()
{
	if(!ui->controlsTree->selectedItems().contains(ui->controlsTree->currentItem()))return 0;
	if(ui->controlsTree->currentItem()->data(0,roleItemType)!=itemTypeGroup)return 0;
	return ui->controlsTree->currentItem();
}

QTreeWidgetItem* ControlsEditor::getCurrentUiControl()
{
	if(!ui->controlsTree->selectedItems().contains(ui->controlsTree->currentItem()))return 0;
	if(ui->controlsTree->currentItem()->data(0,roleItemType)!=itemTypeControl)return 0;
	return ui->controlsTree->currentItem();
}

QTreeWidgetItem* ControlsEditor::getCurrentUiParam()
{
	if(!ui->controlsTree->selectedItems().contains(ui->controlsTree->currentItem()))return 0;
	if(ui->controlsTree->currentItem()->data(0,roleItemType)!=itemTypeParam)return 0;
	return ui->controlsTree->currentItem();
}

QTreeWidgetItem* ControlsEditor::mkUiGroupItem(QTreeWidgetItem *parent,const ControlsGroup &g)
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
		else if(g.elements[i].isCommand())
			mkUiControlItem(item,*g.elements[i].command());
	}

	return item;
}

QTreeWidgetItem* ControlsEditor::mkUiControlItem(QTreeWidgetItem *parent,const ControlsCommand &c)
{
	QTreeWidgetItem *item=new QTreeWidgetItem(parent);
	QFont f=item->font(0);
	f.setBold(true);
	f.setItalic(true);
	item->setFont(0,f);
	item->setFlags(item->flags()|Qt::ItemIsEditable);
	item->setData(0,roleItemType,itemTypeControl);
	ControlsCommand *cmd=new ControlsCommand;
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

QTreeWidgetItem* ControlsEditor::mkUiParamItem(QTreeWidgetItem *parent,const ControlsCommandParam &p)
{
	QTreeWidgetItem *item=new QTreeWidgetItem(parent);
	QFont f=item->font(0);
	item->setFont(0,f);
	item->setFlags(item->flags()|Qt::ItemIsEditable);
	item->setData(0,roleItemType,itemTypeParam);
	ControlsCommandParam *param=new ControlsCommandParam;
	*param=p;
	QSharedPointer<ControlsElement> ptr(param);
	QVariant var;
	var.setValue<QSharedPointer<ControlsElement>>(ptr);
	item->setData(0,roleValue,var);
	item->setText(0,p.title);
	return item;
}

void ControlsEditor::dumpUiGroup(QTreeWidgetItem *item,ControlsGroup &g)
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
			ControlsCommand cc;
			dumpUiCommand(child,cc);
			g.elements.append(ControlsGroup::Element(cc));
		}
	}
}

void ControlsEditor::dumpUiCommand(QTreeWidgetItem *item, ControlsCommand &c)
{
	QSharedPointer<ControlsElement> data=item->data(0,roleValue).value<QSharedPointer<ControlsElement>>();
	c=*((ControlsCommand*)data.data());
	c.title=item->text(0).toUtf8();
	c.params.clear();
	for(int i=0;i<item->childCount();++i)
	{
		QTreeWidgetItem *child=item->child(i);
		int type=child->data(0,roleItemType).toInt();
		if(type!=itemTypeParam)continue;//o_O
		ControlsCommandParam p;
		QSharedPointer<ControlsElement> pData=child->data(0,roleValue).value<QSharedPointer<ControlsElement>>();
		p=*((ControlsCommandParam*)pData.data());
		p.title=child->text(0).toUtf8();
		c.params.append(p);
	}
}

void ControlsEditor::saveCurrentEditedUiItem()
{
	if(!currentEditedUiItem)return;
	uiParamPropsEdit->saveCurrent();
}

void ControlsEditor::rebuildControlUi()
{
	ControlsGroup grp;
	dumpUiGroup(ui->controlsTree->topLevelItem(0),grp);
	ControlUi *controlUi=new ControlUi(&device,grp,ui->controlsView);
	ui->controlsView->setWidget(controlUi);
	controlUi->adjustSize();
}
