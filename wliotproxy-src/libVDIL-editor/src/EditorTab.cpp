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

#include "EditorTab.h"
#include "VDIL/core/Program.h"
#include "VDIL/core/ProgramObject.h"
#include "VDIL/xml/ProgramXmlParser.h"
#include "VDIL/xml/BlocksXmlParserFactory.h"
#include "VDIL/core/SubProgramBlock.h"
#include "VDIL/core/CoreBlocksGroupFactory.h"
#include "VDIL/editor/Editor.h"
#include "SubProgramBlockEditorWidget.h"
#include "BlockGraphicsItem.h"
#include "EditorView.h"
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

EditorTab::EditorTab(WLIOTVDIL::Editor *ed,SubProgram *p,SubProgramBlock *b,QWidget *parent)
	:QWidget(parent)
{
	mEd=ed;
	mSubProgramBlock=b;
	prg=p;
	VDILEditorRcInit::initRc();
	aimCursor=QCursor(QPixmap(":/VDIL/editor/aim.png"));
	drawTmpLink=0;
	edApi=new EditorInternalApi(this);

	scene=new EditorScene(edApi,this);
	view=new EditorView(this);
//	view->setDragMode(QGraphicsView::ScrollHandDrag);
//	view->setCursor(Qt::ArrowCursor);

	QVBoxLayout *mainLayout=new QVBoxLayout(this);
	mainLayout->setContentsMargins(0,0,0,0);
	mainLayout->addWidget(view);

	view->setScene(scene);
	view->setMouseTracking(true);	
}

EditorTab::~EditorTab()
{
	delete edApi;
}

void EditorTab::renderProgram()
{
	for(auto i=blockToItemMap.begin();i!=blockToItemMap.end();++i)
		delete i.value();
	blockToItemMap.clear();
	itemToBlockMap.clear();
	QList<BaseBlock*> blocks=prg->selfBlocks().values();
	if(mSubProgramBlock)
	{
		blocks.append(mSubProgramBlock->internalInputsBlock());
		blocks.append(mSubProgramBlock->internalOutputsBlock());
	}
	for(BaseBlock *b:blocks)
	{
		BlockGraphicsItem *item=new BlockGraphicsItem(b,edApi,
			mEd->mBlocksEditingFactory->editor(b->groupName(),b->blockName())->typeName());
		blockToItemMap[b]=item;
		itemToBlockMap[item]=b;
		scene->addItem(item);
	}
	for(auto i=blockToItemMap.begin();i!=blockToItemMap.end();++i)
		i.value()->createLinks();
}

void EditorTab::onPortLClicked(BlockGraphicsItemPort *port)
{
	if(port->isInput())
		drawTmpLink=new LinkGraphicsItem(0,port,edApi);
	else drawTmpLink=new LinkGraphicsItem(port,0,edApi);
	scene->addItem(drawTmpLink);
	drawTmpLink->setStaticCoordinates(port->mapToScene(port->center));
}

void EditorTab::onLinkRClicked(LinkGraphicsItem *link)
{
	LinkGraphicsItem *t=drawTmpLink;
	undoLinksEditing();
	if(t==link)return;
	if(link->from()&&link->to())
	{
		BlockOutput *out=(BlockOutput*)link->from()->port();
		BlockInput *in=(BlockInput*)link->to()->port();
		out->unlinkFrom(in);
		drawTmpLink=0;
		renderProgram();
	}
}

void EditorTab::onSceneLClicked(QPointF scenePos,QPoint screenPos)
{
	if(!mEd->currentPlacedBlockName.isEmpty())
	{
		BaseBlock *b=mEd->mBlocksFactory->makeBlock(mEd->currentBlocksGroup,mEd->currentPlacedBlockName,0);
		mEd->resetCurrentPlacedBlock();
		if(!b)return;
		IBlockEditor *ed=mEd->mBlocksEditingFactory->editor(b->groupName(),b->blockName());
		if(!ed)
		{
			delete b;
			return;
		}
		if(!b)return;
		b->position=scenePos;
		prg->addBlock(b);
		if(!editBlockSettings(b))
			prg->rmBlock(b->blockId());
	}
}

void EditorTab::onSceneLReleased(QPointF scenePos,QPoint screenPos)
{
	if(drawTmpLink)
	{
		onSceneMouseMove(scenePos,screenPos);
		QList<QGraphicsItem*> items=scene->items(scenePos);
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
			if(!in->supportedTypes().match(out->type()))
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

void EditorTab::onSceneRClicked(QPointF,QPoint)
{
	undoLinksEditing();
}

void EditorTab::onSceneMouseMove(QPointF scenePos,QPoint screenPos)
{
	if(drawTmpLink)
	{
		drawTmpLink->setStaticCoordinates(scenePos);
		scene->update(scene->sceneRect());
	}
}

void EditorTab::onBlockLClicked(BlockGraphicsItem *)
{
}

void EditorTab::onBlockRClicked(BlockGraphicsItem *item)
{
	undoLinksEditing();
	rmBlock(item);
}

void EditorTab::onBlockSettingsClicked(BlockGraphicsItem *item)
{
	if(!editBlockSettings(item->block()))return;
}

void EditorTab::onHeaderLClicked(BlockGraphicsItem *)
{
	undoLinksEditing();
}

void EditorTab::onHeaderDClicked(BlockGraphicsItem *item)
{
	BaseBlock *b=itemToBlockMap.value(item);
	if(b->groupName()==CoreBlocksGroupFactory::mGroupName&&b->blockName()==SubProgramBlock::mBlockName)
		mEd->openSubProgram((SubProgramBlock*)b);
}

void EditorTab::onHeaderRClicked(BlockGraphicsItem *item)
{
	undoLinksEditing();
	rmBlock(item);
}

void EditorTab::onHeaderReleased(BlockGraphicsItem *item)
{
	BaseBlock *b=itemToBlockMap.value(item);
	b->position=item->pos();
	scene->update(scene->sceneRect());
}

void EditorTab::onHeaderMovedBy(BlockGraphicsItem *item,QPointF dist)
{
	item->moveBy(dist.x(),dist.y());
	item->updateLinkPositions();
	scene->update(scene->sceneRect());
}

bool EditorTab::editBlockSettings(BaseBlock *b)
{
	if(b->blockId()==0)return false;//special blocks
	QDialog dlg;
	dlg.setWindowTitle("Block settings");
	IBlockEditor *ed=mEd->mBlocksEditingFactory->editor(b->groupName(),b->blockName());
	if(!ed)return false;
	QWidget *w=ed->mkEditingWidget(mEd->editorHelper,&dlg);

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
	bool subProgramEditContentAfterClose=false;
	if(w&&b->groupName()==CoreBlocksGroupFactory::mGroupName&&b->blockName()==SubProgramBlock::mBlockName)
	{
		SubProgramBlockEditorWidget *ww=(SubProgramBlockEditorWidget*)w;
		connect(ww,&SubProgramBlockEditorWidget::editContent,[&dlg,&subProgramEditContentAfterClose]()
		{
			subProgramEditContentAfterClose=true;
			dlg.accept();
		});
	}
	if(w)ed->loadParamsFromBlock(mEd->editorHelper,w,b);
	titleEdit->setText(b->title);
	if(dlg.exec()!=QDialog::Accepted)return false;
	if(w)ed->saveParamsToBlock(mEd->editorHelper,w,b);
	b->title=titleEdit->text();
	renderProgram();
	if(b->groupName()==CoreBlocksGroupFactory::mGroupName&&b->blockName()==SubProgramBlock::mBlockName)
	{
		mEd->onSubProgramBlockEdited((SubProgramBlock*)b);
		if(subProgramEditContentAfterClose)
		{
			SubProgramBlock *bb=(SubProgramBlock*)b;
			mEd->openSubProgram(bb);
		}
	}
	return true;
}

void EditorTab::rmBlock(BlockGraphicsItem *item)
{
	if(item->block()->blockId()==0)return;//special blocks
	if(QMessageBox::question(this,"Sure?","remove block?")!=QMessageBox::Yes)return;
	BaseBlock *b=item->block();
	if(b->groupName()==CoreBlocksGroupFactory::mGroupName&&b->blockName()==SubProgramBlock::mBlockName)
		mEd->onSubProgramBlockDestroyed((SubProgramBlock*)b);
	prg->rmBlock(item->block()->blockId());
	renderProgram();
}

void EditorTab::undoLinksEditing()
{
	if(drawTmpLink)
	{
		delete drawTmpLink;
		drawTmpLink=0;
	}
}
