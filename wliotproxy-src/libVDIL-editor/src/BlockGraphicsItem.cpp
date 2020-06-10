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

#include "BlockGraphicsItem.h"
#include "EditorInternalApi.h"
#include "BlockGraphicsItemHeader.h"
#include "VDILEditorRcInit.h"
#include <QFont>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QGraphicsScene>

static const double minTitleWidth=80;
static const double blockMargin=10;
static const double portHeight=20;

using namespace WLIOT;
using namespace WLIOTVDIL;

BlockGraphicsItem::BlockGraphicsItem(BaseBlock *block,EditorInternalApi *e,const QString &blockTypeStr)
{
	mBlock=block;
	editor=e;
	mBlockType=blockTypeStr;
	double yOffset=0;

	titleItem=new BlockGraphicsItemHeader(this);
	titleItem->setPos(blockMargin,blockMargin);
	if(mBlock->title.isEmpty())
		titleItem->setText("<untitled>");
	else titleItem->setText(mBlock->title);
	settingsBtnItem=new BlockGraphicsItemConfigButton(QPixmap(":/VDIL/editor/block_settings.png"),this);

	double titleWidth=titleItem->boundingRect().width();
	settingsBtnItem->setPos(titleWidth+2*blockMargin,blockMargin);
	titleWidth+=blockMargin+settingsBtnItem->boundingRect().width();
	if(titleWidth<minTitleWidth)
		titleWidth=minTitleWidth;
	yOffset=2*blockMargin+qMax(titleItem->boundingRect().height(),settingsBtnItem->boundingRect().height());

	typeItem=new QGraphicsSimpleTextItem(this);
	typeItem->setPos(blockMargin,yOffset);
	QFont f=typeItem->font();
	f.setItalic(true);
	f.setBold(true);
	typeItem->setFont(f);
	typeItem->setText(mBlockType);
//	setTextFitToWidth(typeItem,titleWidth,mBlockType);
	typeItem->setToolTip(block->groupName()+"."+block->blockName());
	yOffset+=blockMargin+typeItem->boundingRect().height();

	double totalWidth=qMax(titleWidth,typeItem->boundingRect().width())+blockMargin*2;

	hintItem=new QGraphicsSimpleTextItem(this);
	hintItem->setPos(blockMargin,yOffset);
	setTextFitToWidth(hintItem,titleWidth,e->blockHint(mBlock));
	yOffset+=blockMargin+hintItem->boundingRect().height();

	int inputIndex=0,outputIndex=0;
	bool nextInput=true;
	double portsDist=qMax(hintItem->boundingRect().height(),portHeight)+blockMargin;

	while(true)
	{
		if(inputIndex==block->inputsCount()&&outputIndex==block->outputsCount())break;
		if(nextInput)
		{
			if(inputIndex<block->inputsCount())
			{
				BlockInput *in=block->input(inputIndex);
				BlockGraphicsItemPort *port=new BlockGraphicsItemPort(this,in,true,inputIndex);
				QGraphicsSimpleTextItem *portText=new QGraphicsSimpleTextItem(this);
				setTextFitToWidth(portText,titleWidth-BlockGraphicsItemPort::portSize,in->title());
				port->setPos(0,yOffset);
				portText->setPos(BlockGraphicsItemPort::portSize+blockMargin,yOffset);
				inputPorts.append(port);
				++inputIndex;
				yOffset+=portsDist;
			}
		}
		else
		{
			if(outputIndex<block->outputsCount())
			{
				BlockOutput *out=block->output(outputIndex);
				BlockGraphicsItemPort *port=new BlockGraphicsItemPort(this,out,false,outputIndex);
				QGraphicsSimpleTextItem *portText=new QGraphicsSimpleTextItem(this);
				setTextFitToWidth(portText,titleWidth-BlockGraphicsItemPort::portSize,out->title());
				port->setPos(totalWidth-BlockGraphicsItemPort::portSize,yOffset);
				portText->setPos(blockMargin,yOffset);
				outputPorts.append(port);
				++outputIndex;
				yOffset+=portsDist;
			}
		}
		nextInput=!nextInput;
	}

	bRect.setWidth(totalWidth);
	bRect.setHeight(yOffset);
	setPos(block->position);
}

BlockGraphicsItem::~BlockGraphicsItem()
{
	for(int i=0;i<inputLinks.count();++i)
		delete inputLinks[i];
}

QRectF BlockGraphicsItem::boundingRect()const
{
	return bRect;
}

void BlockGraphicsItem::paint(QPainter *painter,const QStyleOptionGraphicsItem *,QWidget *)
{
	painter->setPen(QPen(Qt::black,3));
	painter->drawRect(bRect);
}

void BlockGraphicsItem::createLinks()
{
	for(int i=0;i<inputLinks.count();++i)
	{
		inputLinks[i]->from()->blockItem()->outputLinks.removeOne(inputLinks[i]);
		delete inputLinks[i];
	}
	inputLinks.clear();
	for(int i=0;i<inputPorts.count();++i)
	{
		BlockGraphicsItemPort *toPort=inputPorts[i];
		BlockInput *in=(BlockInput*)toPort->port();
		if(!in->linkedOutput())continue;
		BaseBlock *linkedBlock=in->linkedOutput()->block();
		BlockGraphicsItem *linkedItem=editor->blockToItemMap().value(linkedBlock);
		if(!linkedItem)continue;

		int outputIndex=linkedBlock->outputIndex(in->linkedOutput());
		BlockGraphicsItemPort *fromPort=linkedItem->outputPorts.value(outputIndex);
		if(!fromPort)continue;
		LinkGraphicsItem *link=new LinkGraphicsItem(fromPort,toPort,editor);
		inputLinks.append(link);
		linkedItem->outputLinks.append(link);
		scene()->addItem(link);
	}
}

QPointF BlockGraphicsItem::inputCenter(int index)
{
	if(index<0||index>=inputPorts.count())
		return QPointF(0,0);
	return inputPorts[index]->mapToScene(BlockGraphicsItemPort::center);
}

QPointF BlockGraphicsItem::outputCenter(int index)
{
	if(index<0||index>=outputPorts.count())
		return QPointF(0,0);
	return outputPorts[index]->mapToScene(BlockGraphicsItemPort::center);
}

void BlockGraphicsItem::setTextFitToWidth(QGraphicsSimpleTextItem *textItem,double w,const QString &text)
{
	QString textCopy=text;
	bool choped=false;
	textItem->setText(textCopy);
	if(textCopy.contains('\n'))
	{
		textCopy=textCopy.left(textCopy.indexOf('\n'));
		textItem->setText(textCopy+"...");
		choped=true;
	}
	while(textItem->boundingRect().width()>w&&!textCopy.isEmpty())
	{
		textCopy.chop(1);
		textItem->setText(textCopy+"...");
		choped=true;
	}
	if(choped)
		textItem->setToolTip(text);
}

void BlockGraphicsItem::updateLinkPositions()
{
	for(LinkGraphicsItem *l:inputLinks)
		l->calcCoordinates();
	for(LinkGraphicsItem *l:outputLinks)
		l->calcCoordinates();
}

BaseBlock* BlockGraphicsItem::block()
{
	return mBlock;
}

void BlockGraphicsItem::onPortLClicked(BlockGraphicsItemPort *port)
{
	editor->onPortLClicked(port);
}

void BlockGraphicsItem::onHeaderLClicked()
{
	editor->onHeaderLClicked(this);
}

void BlockGraphicsItem::onHeaderRClicked()
{
	editor->onHeaderRClicked(this);
}

void BlockGraphicsItem::onHeaderReleased()
{
	editor->onHeaderReleased(this);
}

void BlockGraphicsItem::onHeaderMovedBy(QPointF dist)
{
	editor->onHeaderMovedBy(this,dist);
}

void BlockGraphicsItem::onSettingsClicked()
{
	editor->onBlockSettingsClicked(this);
}
