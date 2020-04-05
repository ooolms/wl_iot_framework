#include "GDIL/editor/BlockGraphicsItemPort.h"
#include "GDIL/editor/BlockGraphicsItem.h"
#include <QPainter>

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
	dimText->setText(QByteArray::number(port->dim()));
	dimText->setPos(5,5);
}

QRectF BlockGraphicsItemPort::boundingRect()const
{
	return bRect;
}

void BlockGraphicsItemPort::paint(QPainter *painter,const QStyleOptionGraphicsItem *,QWidget *)
{
	painter->setPen(QPen(Qt::black,3));
	QColor fillColor=Qt::transparent;
	if(mPort->type()==DataUnit::BOOL)
		fillColor=QColor(255,125,125);
	else if(mPort->type()==DataUnit::ARRAY)
		fillColor=QColor(125,125,255);
	else if(mPort->type()==DataUnit::SINGLE)
		fillColor=QColor(125,255,125);
	painter->setBrush(QBrush(fillColor));
	painter->drawRect(bRect);
}

BlockPort* BlockGraphicsItemPort::port()
{
	return mPort;
}
