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

#include "BlockGraphicsItemPort.h"
#include "BlockGraphicsItem.h"
#include "EditorColors.h"
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

using namespace WLIOT;
using namespace WLIOTVDIL;

const double BlockGraphicsItemPort::portSize=20.0;
const QPointF BlockGraphicsItemPort::center=QPointF(BlockGraphicsItemPort::portSize/2.0,
	BlockGraphicsItemPort::portSize/2.0);
const QRectF BlockGraphicsItemPort::bRect=QRectF(0,0,BlockGraphicsItemPort::portSize,BlockGraphicsItemPort::portSize);

BlockGraphicsItemPort::BlockGraphicsItemPort(BlockGraphicsItem *blockItem,BlockPort *port,bool input,int portIndex)
	:QGraphicsItem(blockItem)
{
	mPortIndex=portIndex;
	mIsInputPort=input;
	mBlockItem=blockItem;
	mPort=port;
	dimText=new QGraphicsSimpleTextItem(this);
	fillBrush=QBrush(Qt::transparent);
	if(mIsInputPort)
	{
		BlockInput *in=(BlockInput*)mPort;
		if(in->supportedTypes().dim!=0)
			dimText->setText(QString::fromUtf8(QByteArray::number(in->supportedTypes().dim)));
		DataUnit::Types tt=in->supportedTypes().types;
		if(tt==0)//???
			fillBrush=QBrush(Qt::white);
		else if(tt==DataUnit::ANY)
			fillBrush=QBrush(EditorColors::anyTypeColor);
		else if(tt==DataUnit::BOOL)
			fillBrush=QBrush(EditorColors::boolTypeColor);
		else if(tt==DataUnit::SINGLE)
			fillBrush=QBrush(EditorColors::singleTypeColor);
		else if(tt==DataUnit::DATETIME)
			fillBrush=QBrush(EditorColors::dateTimeTypeColor);
		else if(tt==DataUnit::ARRAY)
			fillBrush=QBrush(EditorColors::arrayTypeColor);
		else //2 or 3 types of 4
		{
			QColor cl1=Qt::white,cl2=Qt::white,cl3=Qt::white,cl4=Qt::white;
			if(tt&DataUnit::BOOL)
				cl1=EditorColors::boolTypeColor;
			if(tt&DataUnit::SINGLE)
				cl2=EditorColors::singleTypeColor;
			if(tt&DataUnit::ARRAY)
				cl3=EditorColors::arrayTypeColor;
			if(tt&DataUnit::DATETIME)
				cl4=EditorColors::dateTimeTypeColor;
			QPixmap pm(portSize,portSize);
			QPainter p(&pm);
			p.fillRect(0,0,portSize/2,portSize/2,QBrush(cl1));
			p.fillRect(0,portSize/2,portSize/2,portSize/2,QBrush(cl2));
			p.fillRect(portSize/2,0,portSize/2,portSize/2,QBrush(cl3));
			p.fillRect(portSize/2,portSize/2,portSize/2,portSize/2,QBrush(cl4));
			fillBrush.setTexture(pm);
		}
	}
	else
	{
		dimText->setText(QString::fromUtf8(QByteArray::number(port->dim())));
		if(port->type()==DataUnit::BOOL)
			fillBrush=QBrush(EditorColors::boolTypeColor);
		else if(port->type()==DataUnit::SINGLE)
			fillBrush=QBrush(EditorColors::singleTypeColor);
		else if(port->type()==DataUnit::ARRAY)
			fillBrush=QBrush(EditorColors::arrayTypeColor);
		else if(port->type()==DataUnit::DATETIME)
			fillBrush=QBrush(EditorColors::dateTimeTypeColor);
	}
	dimText->setPos(5,3);
}

QRectF BlockGraphicsItemPort::boundingRect()const
{
	return bRect;
}

void BlockGraphicsItemPort::paint(QPainter *painter,const QStyleOptionGraphicsItem *,QWidget *)
{
	painter->setPen(QPen(Qt::black,3));
	painter->setBrush(fillBrush);
	painter->drawRect(bRect);
}

BlockPort* BlockGraphicsItemPort::port()
{
	return mPort;
}

BlockGraphicsItem* BlockGraphicsItemPort::blockItem()
{
	return mBlockItem;
}

bool BlockGraphicsItemPort::isInput()const
{
	return mIsInputPort;
}


void BlockGraphicsItemPort::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mousePressEvent(event);
	if(event->button()==Qt::LeftButton)
	{
		event->ignore();
		mBlockItem->onPortLClicked(this);
	}
}
