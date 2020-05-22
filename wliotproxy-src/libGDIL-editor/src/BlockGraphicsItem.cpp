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
#include "GDILEditorRcInit.h"
#include <QFont>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QGraphicsScene>

static const double minTitleWidth=80;
static const double blockMargin=10;
static const double portHeight=20;

using namespace WLIOT;
using namespace WLIOTGDIL;

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
	settingsBtnItem=new BlockGraphicsItemConfigButton(QPixmap(":/GDIL/editor/block_settings.png"),this);

	double titleWidth=titleItem->boundingRect().width();
	settingsBtnItem->setPos(titleWidth+2*blockMargin,blockMargin);
	titleWidth+=blockMargin+settingsBtnItem->boundingRect().width();
	if(titleWidth<minTitleWidth)
		titleWidth=minTitleWidth;
	double totalWidth=titleWidth+blockMargin*2;
	yOffset=2*blockMargin+qMax(titleItem->boundingRect().height(),settingsBtnItem->boundingRect().height());

	typeItem=new QGraphicsSimpleTextItem(this);
	typeItem->setPos(blockMargin,yOffset);
	QFont f=typeItem->font();
	f.setItalic(true);
	typeItem->setFont(f);
	setTextFitToWidth(typeItem,titleWidth,mBlockType);
	typeItem->setToolTip(block->groupName()+"."+block->blockName());
	yOffset+=blockMargin+typeItem->boundingRect().height();

	hintItem=new QGraphicsSimpleTextItem(this);
	hintItem->setPos(blockMargin,yOffset);
	setTextFitToWidth(hintItem,titleWidth,e->blockHint(mBlock));
	double portUnitHeight=qMax(hintItem->boundingRect().height(),portHeight);
	yOffset+=blockMargin+hintItem->boundingRect().height();

	double outputsOffset=yOffset+portUnitHeight+blockMargin;
	double portsDist=2*(portUnitHeight+blockMargin);

	for(int i=0;i<block->inputsCount();++i)
	{
		BlockInput *in=block->input(i);
		BlockGraphicsItemPort *port=new BlockGraphicsItemPort(this,in,true,i);
		QGraphicsSimpleTextItem *portText=new QGraphicsSimpleTextItem(this);
		setTextFitToWidth(portText,titleWidth-BlockGraphicsItemPort::portSize,in->title());
		port->setPos(0,yOffset+portsDist*i);
		portText->setPos(BlockGraphicsItemPort::portSize+blockMargin,yOffset+portsDist*i);
		inputPorts.append(port);
	}

	for(int i=0;i<block->outputsCount();++i)
	{
		BlockOutput *out=block->output(i);
		BlockGraphicsItemPort *port=new BlockGraphicsItemPort(this,block->output(i),false,i);
		QGraphicsSimpleTextItem *portText=new QGraphicsSimpleTextItem(this);
		setTextFitToWidth(portText,titleWidth-BlockGraphicsItemPort::portSize,out->title());
		port->setPos(totalWidth-BlockGraphicsItemPort::portSize,outputsOffset+portsDist*i);
		portText->setPos(blockMargin,outputsOffset+portsDist*i);
		outputPorts.append(port);
	}

	bRect.setWidth(totalWidth);
	if(block->inputsCount()!=0||block->outputsCount()!=0)
	{
		bRect.setHeight(qMax(yOffset+portsDist*block->inputsCount()-portsDist/2.0,
			outputsOffset+portsDist*block->outputsCount()-portsDist/2.0));
	}
	else bRect.setHeight(yOffset);
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
