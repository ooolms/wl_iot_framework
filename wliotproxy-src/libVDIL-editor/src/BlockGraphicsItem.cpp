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

static const double minTitleWidthWithTitle=40;
static const double titleMargin=8;
static const double blockMargin=4;

using namespace WLIOT;
using namespace WLIOTVDIL;

BlockGraphicsItem::BlockGraphicsItem(BaseBlock *block,EditorInternalApi *e,const QString &blockTypeStr)
{
	mBlock=block;
	editor=e;
	mBlockType=blockTypeStr;

	double minTitleWidth=0;
	double yOffset=0;

	titleItem=new BlockGraphicsItemHeader(this);
	titleItem->setPos(titleMargin,titleMargin);
	if(mBlock->title.isEmpty())
		titleItem->setText("<>");
	else
	{
		minTitleWidth=minTitleWidthWithTitle;
		titleItem->setText(mBlock->title);
	}
	settingsBtnItem=new BlockGraphicsItemConfigButton(QPixmap(":/VDIL/editor/block_settings.png"),this);
	settingsBtnItem->setToolTip(mBlockType);

	double titleWidth=titleItem->boundingRect().width();
	if(titleWidth<minTitleWidth)
		titleWidth=minTitleWidth;
	settingsBtnItem->setPos(titleWidth+titleMargin+blockMargin,titleMargin);

	yOffset=titleMargin+blockMargin+titleItem->boundingRect().height();
	hintItem=new QGraphicsSimpleTextItem(this);
	hintItem->setPos(blockMargin,yOffset);
	setTextFitToWidth(hintItem,titleWidth,editor->blockHint(mBlock));

	double totalWidth=titleWidth+blockMargin+2*titleMargin+settingsBtnItem->boundingRect().width();
	yOffset=titleMargin+qMax(settingsBtnItem->boundingRect().height(),
		titleItem->boundingRect().height()+blockMargin+hintItem->boundingRect().height())+blockMargin;

	placePortsExternal(totalWidth,yOffset);
}

BlockGraphicsItem::~BlockGraphicsItem()
{
	for(int i=0;i<inputLinks.count();++i)
		delete inputLinks[i];
}

QRectF BlockGraphicsItem::boundingRect()const
{
	return mBoundingRect;
}

void BlockGraphicsItem::paint(QPainter *painter,const QStyleOptionGraphicsItem *,QWidget *)
{
	painter->setPen(QPen(Qt::black,3));
	painter->drawRoundedRect(blockRect,8,8);
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
		BlockGraphicsItem *linkedItem=editor->itemForBlock(linkedBlock);
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

void BlockGraphicsItem::onHeaderDClicked()
{
	editor->onHeaderDClicked(this);
}

void BlockGraphicsItem::placePortsExternal(double totalWidth,double yOffset)
{
	int inputIndex=0,outputIndex=0;
	bool nextInput=true;
	double portsDist=qMax(hintItem->boundingRect().height(),BlockGraphicsItemPort::portSize)+blockMargin;
	double textWidth=totalWidth-2*blockMargin;

	while(true)
	{
		if(inputIndex==mBlock->inputsCount()&&outputIndex==mBlock->outputsCount())break;
		if(nextInput)
		{
			if(inputIndex<mBlock->inputsCount())
			{
				BlockInput *in=mBlock->input(inputIndex);
				BlockGraphicsItemPort *port=new BlockGraphicsItemPort(this,in,true,inputIndex);
				QGraphicsSimpleTextItem *portText=new QGraphicsSimpleTextItem(this);
				setTextFitToWidth(portText,textWidth,in->title());
				port->setPos(-BlockGraphicsItemPort::portSize,yOffset);
				portText->setPos(blockMargin,yOffset+
					(port->boundingRect().height()-portText->boundingRect().height())/2);
				inputPorts.append(port);
				++inputIndex;
			}
			yOffset+=portsDist;
		}
		else
		{
			if(outputIndex<mBlock->outputsCount())
			{
				BlockOutput *out=mBlock->output(outputIndex);
				BlockGraphicsItemPort *port=new BlockGraphicsItemPort(this,out,false,outputIndex);
				QGraphicsSimpleTextItem *portText=new QGraphicsSimpleTextItem(this);
				setTextFitToWidth(portText,textWidth,out->title());
				port->setPos(totalWidth,yOffset);
				portText->setPos(totalWidth-blockMargin-portText->boundingRect().width(),yOffset+
					(port->boundingRect().height()-portText->boundingRect().height())/2);
				outputPorts.append(port);
				++outputIndex;
			}
			yOffset+=portsDist;
		}
		nextInput=!nextInput;
	}
	blockRect=QRectF(0,0,totalWidth,yOffset);
	mBoundingRect=QRectF(-BlockGraphicsItemPort::portSize,0,totalWidth+2*BlockGraphicsItemPort::portSize,yOffset);
	setPos(mBlock->position);
}

void BlockGraphicsItem::placePortsInternal(double totalWidth,double yOffset)
{
	int inputIndex=0,outputIndex=0;
	bool nextInput=true;
	double portsDist=qMax(hintItem->boundingRect().height(),BlockGraphicsItemPort::portSize)+blockMargin;
	double textWidth=totalWidth-BlockGraphicsItemPort::portSize-2*blockMargin;

	while(true)
	{
		if(inputIndex==mBlock->inputsCount()&&outputIndex==mBlock->outputsCount())break;
		if(nextInput)
		{
			if(inputIndex<mBlock->inputsCount())
			{
				BlockInput *in=mBlock->input(inputIndex);
				BlockGraphicsItemPort *port=new BlockGraphicsItemPort(this,in,true,inputIndex);
				QGraphicsSimpleTextItem *portText=new QGraphicsSimpleTextItem(this);
				setTextFitToWidth(portText,textWidth,in->title());
				port->setPos(0,yOffset);
				portText->setPos(BlockGraphicsItemPort::portSize+blockMargin,yOffset+
					(port->boundingRect().height()-portText->boundingRect().height())/2);
				inputPorts.append(port);
				++inputIndex;
				yOffset+=portsDist;
			}
		}
		else
		{
			if(outputIndex<mBlock->outputsCount())
			{
				BlockOutput *out=mBlock->output(outputIndex);
				BlockGraphicsItemPort *port=new BlockGraphicsItemPort(this,out,false,outputIndex);
				QGraphicsSimpleTextItem *portText=new QGraphicsSimpleTextItem(this);
				setTextFitToWidth(portText,textWidth,out->title());
				port->setPos(totalWidth-BlockGraphicsItemPort::portSize,yOffset);
				portText->setPos(totalWidth-BlockGraphicsItemPort::portSize-blockMargin-
					portText->boundingRect().width(),yOffset+(port->boundingRect().height()-
					portText->boundingRect().height())/2);
				outputPorts.append(port);
				++outputIndex;
				yOffset+=portsDist;
			}
		}
		nextInput=!nextInput;
	}
	mBoundingRect.setWidth(totalWidth);
	mBoundingRect.setHeight(yOffset);
	blockRect=mBoundingRect;
	setPos(mBlock->position);
}
