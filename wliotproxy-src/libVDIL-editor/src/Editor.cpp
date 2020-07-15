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
#include "VDIL/xml/ProgramXmlParser.h"
#include "VDIL/xml/BlocksXmlParserFactory.h"
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

using namespace WLIOT;
using namespace WLIOTVDIL;

Editor::Editor(BlocksFactory *blocksFact,BlocksXmlParserFactory *blocksXmlFact,
	BlocksEditingFactory *blocksEdFact,IEditorHelper *hlp,QWidget *parent)
	:QWidget(parent)
{
	VDILEditorRcInit::initRc();
	editorHelper=hlp;
	aimCursor=QCursor(QPixmap(":/VDIL/editor/aim.png"));
	drawTmpLink=0;
	edApi=new EditorInternalApi(this);
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
	blocksToolbar->setIconSize(QSize(96,96));

	scene=new EditorScene(edApi,this);
	view=new QGraphicsView(splitter);

	splitter->addWidget(leftWidget);
	splitter->addWidget(view);
	splitter->setSizes(QList<int>()<<50<<200);

	QVBoxLayout *leftLayout=new QVBoxLayout(leftWidget);
	leftLayout->addWidget(editConfigBtn);
	leftLayout->addWidget(blocksGroupSelect);
	leftLayout->addWidget(blocksToolbar,1);

	QHBoxLayout *mainLayout=new QHBoxLayout(this);
	mainLayout->addWidget(splitter);

	view->setScene(scene);
	view->setMouseTracking(true);

	connect(blocksGroupSelect,static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
		this,&Editor::onBlocksGroupSelected);
	connect(blocksToolbar,&QTreeWidget::itemSelectionChanged,this,&Editor::onBlocksToolbarSelChanged);
	connect(prgObj,&ProgramObject::execCommand,this,&Editor::execCommand);
	connect(prgObj,&ProgramObject::debugMessage,this,&Editor::debugMessage);
	connect(editConfigBtn,&QPushButton::clicked,this,&Editor::onEditConfigClicked);

	onBlocksGroupSelected(blocksGroupSelect->currentIndex());
}

Editor::~Editor()
{
	delete prgObj;
	delete prg;
	delete edApi;
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
	delete prg;
	prg=p;
	prgObj->setProgram(prg);
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
	if(!currentPlacedBlockName.isNull())
		toolbarTypeToActionMap[currentPlacedBlockName]->setSelected(false);
	currentPlacedBlockName=type;
	setCursor(aimCursor);
	toolbarTypeToActionMap[currentPlacedBlockName]->setSelected(true);
}

void Editor::onBlocksGroupSelected(int index)
{
	currentPlacedBlockName.clear();
	setCursor(Qt::ArrowCursor);
	blocksToolbar->clear();
	toolbarActionToTypeMap.clear();
	toolbarTypeToActionMap.clear();
	currentBlocksGroup=blocksGroupSelect->itemData(index,Qt::UserRole).toString();
	IBlocksGroupEditorsFactory *f=mBlocksEditingFactory->groupFactory(currentBlocksGroup);
	if(!f)return;
	for(QString blockName:f->allBlocks())
	{
		IBlockEditor *e=f->editor(blockName);
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

void Editor::renderProgram()
{
	for(auto i=blockToItemMap.begin();i!=blockToItemMap.end();++i)
		delete i.value();
	blockToItemMap.clear();
	itemToBlockMap.clear();
	for(auto i=prg->allBlocks().begin();i!=prg->allBlocks().end();++i)
	{
		BaseBlock *b=i.value();
		BlockGraphicsItem *item=new BlockGraphicsItem(b,edApi,
			mBlocksEditingFactory->editor(b->groupName(),b->blockName())->typeName());
		blockToItemMap[b]=item;
		itemToBlockMap[item]=b;
		scene->addItem(item);
	}
	for(auto i=blockToItemMap.begin();i!=blockToItemMap.end();++i)
		i.value()->createLinks();
}

void Editor::onPortLClicked(BlockGraphicsItemPort *port)
{
	if(drawTmpLink)
		delete drawTmpLink;
	if(port->isInput())
		drawTmpLink=new LinkGraphicsItem(0,port,edApi);
	else drawTmpLink=new LinkGraphicsItem(port,0,edApi);
	scene->addItem(drawTmpLink);
	drawTmpLink->setStaticCoordinates(port->mapToScene(port->center));
}

void Editor::onLinkRClicked(LinkGraphicsItem *link)
{
	if(link->from()&&link->to())
	{
		BlockOutput *out=(BlockOutput*)link->from()->port();
		BlockInput *in=(BlockInput*)link->to()->port();
		out->unlinkFrom(in);
		renderProgram();
	}
}

void Editor::onSceneLClicked(QPointF pos)
{
	if(!currentPlacedBlockName.isEmpty())
	{
		BaseBlock *b=mBlocksFactory->makeBlock(currentBlocksGroup,currentPlacedBlockName,0);
		toolbarTypeToActionMap[currentPlacedBlockName]->setSelected(false);
		currentPlacedBlockName.clear();
		IBlockEditor *ed=mBlocksEditingFactory->editor(b->groupName(),b->blockName());
		if(!ed)
		{
			delete b;
			return;
		}
		setCursor(Qt::ArrowCursor);
		if(!b)return;
		b->position=pos;
		prg->addBlock(b);
		if(!editBlockSettings(b))
			prg->rmBlock(b->blockId());
	}
}

void Editor::onSceneLReleased(QPointF pos)
{
	if(drawTmpLink)
	{
		QList<QGraphicsItem*> items=scene->items(pos);
		BlockGraphicsItemPort *dropPort=0;
		for(QGraphicsItem *i:items)
		{
			if(typeid(*i)==typeid(BlockGraphicsItemPort))
				dropPort=(BlockGraphicsItemPort*)i;
		}
		if(dropPort)
		{
			BlockGraphicsItemPort *from=drawTmpLink->from();
			BlockGraphicsItemPort *to=drawTmpLink->to();
			delete drawTmpLink;
			drawTmpLink=0;
			if(from&&dropPort->isInput())
				to=dropPort;
			else if(to&&!dropPort->isInput())
				from=dropPort;
			else return;
			if(from->blockItem()==to->blockItem())return;
			BlockOutput *out=(BlockOutput*)from->port();
			BlockInput *in=(BlockInput*)to->port();
			if(in->linkedOutput())
				return;
			if(!in->supportedTypes().match(out->type(),out->dim()))
				return;
			if(!out->linkTo(in))
				return;
			renderProgram();
		}
		else
		{
			delete drawTmpLink;
			drawTmpLink=0;
			scene->update(scene->sceneRect());
		}
	}
}

void Editor::onSceneRClicked(QPointF)
{
}

void Editor::onSceneMouseMove(QPointF pos)
{
	if(drawTmpLink)
	{
		drawTmpLink->setStaticCoordinates(pos);
		scene->update(scene->sceneRect());
	}
}

void Editor::onBlockLClicked(BlockGraphicsItem *)
{
}

void Editor::onBlockRClicked(BlockGraphicsItem *item)
{
	if(QMessageBox::question(this,"Sure?","remove block?")!=QMessageBox::Yes)return;
	prg->rmBlock(item->block()->blockId());
	renderProgram();
}

void Editor::onBlockSettingsClicked(BlockGraphicsItem *item)
{
	editBlockSettings(item->block());
}

void Editor::onHeaderLClicked(BlockGraphicsItem *)
{
}

void Editor::onHeaderRClicked(BlockGraphicsItem *item)
{
	if(QMessageBox::question(this,"Sure?","remove block?")!=QMessageBox::Yes)return;
	prg->rmBlock(item->block()->blockId());
	renderProgram();
}

void Editor::onHeaderReleased(BlockGraphicsItem *item)
{
	BaseBlock *b=itemToBlockMap.value(item);
	b->position=item->pos();
	scene->update(scene->sceneRect());
}

void Editor::onHeaderMovedBy(BlockGraphicsItem *item,QPointF dist)
{
	item->moveBy(dist.x(),dist.y());
	item->updateLinkPositions();
	scene->update(scene->sceneRect());
}

bool Editor::editBlockSettings(BaseBlock *b)
{
	QDialog dlg;
	dlg.setWindowTitle("Block settings");
	IBlockEditor *ed=mBlocksEditingFactory->editor(b->groupName(),b->blockName());
	if(!ed)return false;
	QWidget *w=ed->mkEditingWidget(editorHelper,&dlg);

	QWidget *tw=new QWidget(&dlg);
	QLabel *titleLabel=new QLabel("title",tw);
	QLineEdit *titleEdit=new QLineEdit(tw);
	QDialogButtonBox *btns=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,&dlg);

	QVBoxLayout *lay=new QVBoxLayout(&dlg);
	lay->addWidget(tw);
	if(w)lay->addWidget(w,1);
	lay->addWidget(btns);

	QHBoxLayout *twLay=new QHBoxLayout(tw);
	twLay->setContentsMargins(0,0,0,0);
	twLay->addWidget(titleLabel);
	twLay->addWidget(titleEdit,1);

	connect(btns,&QDialogButtonBox::accepted,&dlg,&QDialog::accept);
	connect(btns,&QDialogButtonBox::rejected,&dlg,&QDialog::reject);
	if(w)ed->loadParamsFromBlock(editorHelper,w,b);
	titleEdit->setText(b->title);
	if(dlg.exec()!=QDialog::Accepted)return false;
	if(w)ed->saveParamsToBlock(editorHelper,w,b);
	b->title=titleEdit->text();
	renderProgram();
	return true;
}

bool Editor::eventFilter(QObject *watched,QEvent *event)
{
	if(watched==blocksToolbar&&event->type()==QEvent::KeyPress)
	{
		if(currentPlacedBlockName.isEmpty())return false;
		if((event->type()==QEvent::KeyPress&&((QKeyEvent*)event)->key()==Qt::Key_Escape)||
			(event->type()==QEvent::MouseButtonPress&&((QMouseEvent*)event)->button()==Qt::RightButton))
		{
			blocksToolbar->clearSelection();
			currentPlacedBlockName.clear();
			setCursor(Qt::ArrowCursor);
			return true;
		}
	}
	return false;
}
