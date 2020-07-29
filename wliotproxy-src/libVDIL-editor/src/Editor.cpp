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

#include "VDIL/editor/Editor.h"
#include "VDIL/core/Program.h"
#include "VDIL/core/ProgramObject.h"
#include "VDIL/core/SubProgramBlock.h"
#include "VDIL/xml/ProgramXmlParser.h"
#include "VDIL/xml/BlocksXmlParserFactory.h"
#include "EditorTab.h"
#include "BlockGraphicsItem.h"
#include <QLayout>
#include "EditorInternalApi.h"
#include "EditorScene.h"
#include "VDILEditorRcInit.h"
#include "ProgramConfigDialog.h"
#include <QMessageBox>
#include <QGraphicsView>
#include <QLineEdit>
#include <QLabel>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLayout>
#include <QSplitter>
#include <QDebug>
#include <QScrollBar>
#include <QComboBox>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPushButton>
#include <QTabWidget>

using namespace WLIOT;
using namespace WLIOTVDIL;

Editor::Editor(BlocksFactory *blocksFact,BlocksXmlParserFactory *blocksXmlFact,
	BlocksEditingFactory *blocksEdFact,IEditorHelper *hlp,QWidget *parent)
	:QWidget(parent)
{
	VDILEditorRcInit::initRc();
	editorHelper=hlp;
	prg=new Program;
	prgObj=new ProgramObject(0,this);
	prgObj->setProgram(prg);
	mBlocksFactory=blocksFact;
	mBlocksXmlParserFactory=blocksXmlFact;
	mBlocksEditingFactory=blocksEdFact;

	QSplitter *splitter=new QSplitter(this);

	QWidget *leftWidget=new QWidget(splitter);
	editConfigBtn=new QPushButton("program configuration",leftWidget);
	blocksGroupSelect=new QComboBox(leftWidget);
	for(QString &s:mBlocksEditingFactory->allGroups())
	{
		IBlocksGroupEditorsFactory *f=mBlocksEditingFactory->groupFactory(s);
		blocksGroupSelect->addItem(f->groupDisplayTitle(),s);
	}
	blocksGroupSelect->setCurrentIndex(0);

	blocksToolbar=new QTreeWidget(leftWidget);
	blocksToolbar->setHeaderHidden(true);
	blocksToolbar->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	blocksToolbar->verticalScrollBar()->setSingleStep(16);
	blocksToolbar->installEventFilter(this);
	blocksToolbar->setIconSize(QSize(64,64));

	tabs=new QTabWidget(splitter);
	tabs->setTabsClosable(true);
	tabs->setUsesScrollButtons(true);
	mainTab=new EditorTab(this,prg,0,tabs);
	tabs->addTab(mainTab,"Main");

	splitter->addWidget(leftWidget);
	splitter->addWidget(tabs);
	splitter->setSizes(QList<int>()<<50<<200);

	QVBoxLayout *leftLayout=new QVBoxLayout(leftWidget);
	leftLayout->addWidget(editConfigBtn);
	leftLayout->addWidget(blocksGroupSelect);
	leftLayout->addWidget(blocksToolbar,1);

	QHBoxLayout *mainLayout=new QHBoxLayout(this);
	mainLayout->addWidget(splitter);

	connect(blocksGroupSelect,static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
		this,&Editor::onBlocksGroupSelected);
	connect(blocksToolbar,&QTreeWidget::itemSelectionChanged,this,&Editor::onBlocksToolbarSelChanged);
	connect(prgObj,&ProgramObject::execCommand,this,&Editor::execCommand);
	connect(prgObj,&ProgramObject::debugMessage,this,&Editor::debugMessage);
	connect(editConfigBtn,&QPushButton::clicked,this,&Editor::onEditConfigClicked);
	connect(tabs,&QTabWidget::tabCloseRequested,this,&Editor::onCloseTabRequest);

	onBlocksGroupSelected(blocksGroupSelect->currentIndex());
}

Editor::~Editor()
{
	delete prgObj;
	delete prg;
}

bool Editor::setProgram(const QByteArray &xmlData)
{
	Program *p;
	if(xmlData.isEmpty())
		p=new Program;
	else p=ProgramXmlParser::fromXml(mBlocksXmlParserFactory,mBlocksFactory,xmlData,false);
	if(!p)
	{
		if(QMessageBox::question(this,"Program parsing errors","Try ignore errors?")!=QMessageBox::Yes)
			return false;
		p=ProgramXmlParser::fromXml(mBlocksXmlParserFactory,mBlocksFactory,xmlData,true);
		if(!p)return false;
	}
	for(auto i=p->allBlocks().begin();i!=p->allBlocks().end();++i)
	{
		if(!mBlocksEditingFactory->editor(i.value()->groupName(),i.value()->blockName()))
		{
			delete p;
			return false;
		}
	}
	tabs->clear();
	for(EditorTab *t:mTabsMap)
		delete t;
	delete mainTab;
	mTabsMap.clear();
	delete prg;
	prg=p;
	prgObj->setProgram(prg);
	mainTab=new EditorTab(this,prg,0,tabs);
	tabs->addTab(mainTab,"Main");
	renderProgram();
	return true;
}

QByteArray Editor::getProgram()
{
	return ProgramXmlParser::toXml(mBlocksXmlParserFactory,prg);
}

void Editor::onBlocksToolbarSelChanged()
{
	if(!blocksToolbar->selectedItems().contains(blocksToolbar->currentItem()))
		return;
	QTreeWidgetItem *item=blocksToolbar->currentItem();
	QString type=toolbarActionToTypeMap.value(item);
	if(type.isNull())return;
	if(!currentPlacedBlockName.isEmpty())
		toolbarTypeToActionMap[currentPlacedBlockName]->setSelected(false);
	currentPlacedBlockName=type;
	setCursor(aimCursor);
	toolbarTypeToActionMap[currentPlacedBlockName]->setSelected(true);
}

void Editor::onBlocksGroupSelected(int index)
{
	resetCurrentPlacedBlock();
	blocksToolbar->clear();
	toolbarActionToTypeMap.clear();
	toolbarTypeToActionMap.clear();
	currentBlocksGroup=blocksGroupSelect->itemData(index,Qt::UserRole).toString();
	IBlocksGroupEditorsFactory *f=mBlocksEditingFactory->groupFactory(currentBlocksGroup);
	if(!f)return;
	for(QString blockName:f->allBlocks())
	{
		IBlockEditor *e=f->editor(blockName);
		if(e->treeName().isEmpty())continue;
		QTreeWidgetItem *item=new QTreeWidgetItem(blocksToolbar);
		item->setText(0,e->treeName());
		item->setIcon(0,e->previewImage());
		item->setToolTip(0,e->description());
		toolbarActionToTypeMap[item]=blockName;
		toolbarTypeToActionMap[blockName]=item;
	}
}

void Editor::onEditConfigClicked()
{
	ProgramConfigDialog dlg(this);
	dlg.loadSettings(prg);
	if(dlg.exec()!=QDialog::Accepted)return;
	dlg.storeSettings(prg);
	renderProgram();
}

void Editor::onCloseTabRequest(int index)
{
	EditorTab *t=(EditorTab*)tabs->widget(index);
	if(t==mainTab)return;
	SubProgramBlock *b=mTabsMap.key(t);
	if(!b)return;
	mTabsMap.remove(b);
	tabs->removeTab(index);
	delete t;
}

void Editor::renderProgram()
{
	mainTab->renderProgram();
	for(EditorTab *t:mTabsMap)
		t->renderProgram();
}

void Editor::resetCurrentPlacedBlock()
{
	blocksToolbar->clearSelection();
	currentPlacedBlockName.clear();
	setCursor(Qt::ArrowCursor);
}

void Editor::onSubProgramBlockEdited(SubProgramBlock *b)
{
	EditorTab *t=mTabsMap.value(b);
	if(!t)return;
	tabs->setTabText(tabs->indexOf(t),b->title);
	t->renderProgram();
}

void Editor::onSubProgramBlockDestroyed(SubProgramBlock *b)
{
	EditorTab *t=mTabsMap.take(b);
	if(!t)return;
	tabs->removeTab(tabs->indexOf(t));
	delete t;
}

void Editor::openSubProgram(SubProgramBlock *b)
{
	if(mTabsMap.contains(b))
	{
		tabs->setCurrentIndex(tabs->indexOf(mTabsMap[b]));
		return;
	}
	EditorTab *t=new EditorTab(this,b->subProgram(),b,tabs);
	tabs->addTab(t,b->title);
	mTabsMap[b]=t;
	t->renderProgram();
	tabs->setCurrentIndex(tabs->count()-1);
}

bool Editor::eventFilter(QObject *watched,QEvent *event)
{
	if(watched==blocksToolbar&&event->type()==QEvent::KeyPress)
	{
		if(currentPlacedBlockName.isEmpty())return false;
		if((event->type()==QEvent::KeyPress&&((QKeyEvent*)event)->key()==Qt::Key_Escape)||
			(event->type()==QEvent::MouseButtonPress&&((QMouseEvent*)event)->button()==Qt::RightButton))
		{
			resetCurrentPlacedBlock();
			return true;
		}
	}
	return false;
}
