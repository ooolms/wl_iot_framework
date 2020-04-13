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

#include "GDIL/editor/Editor.h"
#include "GDIL/core/Program.h"
#include "GDIL/xml/ProgramXmlParser.h"
#include "GDIL/xml/BlocksXmlParserFactory.h"
#include "BlockGraphicsItem.h"
#include <QLayout>
#include "EditorInternalApi.h"
#include "EditorScene.h"
#include "GDILEditorRcInit.h"
#include <QMessageBox>
#include <QGraphicsView>
#include <QLineEdit>
#include <QLabel>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLayout>
#include <QDebug>
#include <QScrollBar>
#include <QKeyEvent>

Editor::Editor(BlocksFactory *blocksFact,BlocksXmlParserFactory *blocksXmlFact,
	BlocksEditingFactory *blocksEdFact,QWidget *parent)
	:QWidget(parent)
{
	GDILEditorRcInit::initRc();
	drawTmpLink=0;
	edApi=new EditorInternalApi(this);
	prg=new Program;
	mBlocksFactory=blocksFact;
	mBlocksXmlParserFactory=blocksXmlFact;
	mBlocksEditingFactory=blocksEdFact;
	scene=new EditorScene(edApi,this);
	view=new QGraphicsView(this);
	blocksToolbar=new QTreeWidget(this);
	blocksToolbar->setHeaderHidden(true);
	blocksToolbar->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	blocksToolbar->verticalScrollBar()->setSingleStep(16);
	blocksToolbar->installEventFilter(this);

	QHBoxLayout *mainLayout=new QHBoxLayout(this);
	mainLayout->addWidget(blocksToolbar);
	mainLayout->addWidget(view,1);

	view->setScene(scene);
	view->setMouseTracking(true);

	blocksToolbar->setIconSize(QSize(128,128));
	for(QUuid type:blocksEdFact->allSupportedTypes())
	{
		IBlockEditor *e=blocksEdFact->editorByType(type);
		QTreeWidgetItem *item=new QTreeWidgetItem(blocksToolbar);
		item->setIcon(0,e->previewImage());
		item->setToolTip(0,e->description());
		toolbarActionToTypeMap[item]=type;
		toolbarTypeToActionMap[type]=item;
	}

	connect(blocksToolbar,&QTreeWidget::itemSelectionChanged,this,&Editor::onBlocksToolbarSelChanged);
}

Editor::~Editor()
{
	delete prg;
	delete edApi;
}

bool Editor::setProgram(const QByteArray &xmlData)
{
	Program *p=ProgramXmlParser::fromXml(mBlocksXmlParserFactory,mBlocksFactory,xmlData);
	if(!p)return false;
	for(auto i=p->sourceBlocks().begin();i!=p->sourceBlocks().end();++i)
	{
		if(!mBlocksEditingFactory->hasEditor(i.value()->typeId()))
		{
			delete p;
			return false;
		}
	}
	for(auto i=p->processingBlocks().begin();i!=p->processingBlocks().end();++i)
	{
		if(!mBlocksEditingFactory->hasEditor(i.value()->typeId()))
		{
			delete p;
			return false;
		}
	}
	delete prg;
	prg=p;
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
	QUuid type=toolbarActionToTypeMap.value(item);
	if(type.isNull())return;
	if(!currentPlacedBlockType.isNull())
		toolbarTypeToActionMap[currentPlacedBlockType]->setSelected(false);
	currentPlacedBlockType=type;
	setCursor(Qt::CrossCursor);
	toolbarTypeToActionMap[currentPlacedBlockType]->setSelected(true);
}

void Editor::renderProgram()
{
	for(auto i=blockToItemMap.begin();i!=blockToItemMap.end();++i)
		delete i.value();
	blockToItemMap.clear();
	itemToBlockMap.clear();
	for(auto i=prg->sourceBlocks().begin();i!=prg->sourceBlocks().end();++i)
	{
		BaseBlock *b=i.value();
		BlockGraphicsItem *item=new BlockGraphicsItem(
			b,edApi,mBlocksEditingFactory->editorByType(b->typeId())->typeName());
		blockToItemMap[b]=item;
		itemToBlockMap[item]=b;
		scene->addItem(item);
	}
	for(auto i=prg->processingBlocks().begin();i!=prg->processingBlocks().end();++i)
	{
		BaseBlock *b=i.value();
		BlockGraphicsItem *item=new BlockGraphicsItem(b,edApi,
			mBlocksEditingFactory->editorByType(b->typeId())->typeName());
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
		link->to()->blockItem()->createLinks();
	}
}

void Editor::onSceneLClicked(QPointF pos)
{
	if(!currentPlacedBlockType.isNull())
	{
		BaseBlock *b=mBlocksFactory->makeBlock(currentPlacedBlockType);
		toolbarTypeToActionMap[currentPlacedBlockType]->setSelected(false);
		currentPlacedBlockType=QUuid();
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
			else if(drawTmpLink->to()&&!dropPort->isInput())
				from=dropPort;
			else return;
			BlockOutput *out=(BlockOutput*)from->port();
			BlockInput *in=(BlockInput*)to->port();
			if(in->linkedOutput())
				return;
			if(!in->canConnectType(out->type(),out->dim()))
				return;
			if(!out->linkTo(in))
				return;
			BlockGraphicsItem *toItem=blockToItemMap.value(in->block());
			toItem->createLinks();
			scene->update(scene->sceneRect());
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
	IBlockEditor *ed=mBlocksEditingFactory->editorByType(b->typeId());
	QWidget *w=ed->mkEditingWidget(edApi,&dlg);

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
	if(w)ed->loadParamsFromBlock(w,b);
	titleEdit->setText(b->title);
	if(dlg.exec()!=QDialog::Accepted)return false;
	if(w)ed->saveParamsToBlock(w,b);
	b->title=titleEdit->text();
	renderProgram();
	return true;
}

bool Editor::eventFilter(QObject *watched,QEvent *event)
{
	if(watched==blocksToolbar&&event->type()==QEvent::KeyPress)
	{
		QKeyEvent *kEv=(QKeyEvent*)event;
		if(kEv->key()==Qt::Key_Escape)
		{
			blocksToolbar->clearSelection();
			currentPlacedBlockType=QUuid();
			setCursor(Qt::ArrowCursor);
			return true;
		}
	}
	return false;
}
