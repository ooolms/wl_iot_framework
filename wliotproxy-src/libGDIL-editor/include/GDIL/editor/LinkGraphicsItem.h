#ifndef LINKGRAPHICSITEM_H
#define LINKGRAPHICSITEM_H

#include <QGraphicsItem>

class BlockGraphicsItemPort;

class LinkGraphicsItem
	:public QGraphicsItem
{
public:
	explicit LinkGraphicsItem(BlockGraphicsItemPort *from,BlockGraphicsItemPort *to);
	virtual QRectF boundingRect()const override;
	virtual QPainterPath shape()const override;
	virtual void paint(QPainter *painter,const QStyleOptionGraphicsItem *,QWidget *)override;

public:
	BlockGraphicsItemPort *mFromPort,*mToPort;
	QPointF mFrom,mTo;
	double angle;
	QRectF bRect;
	QPointF arrStartDrawPoint,arrEndDrawPoint;
	QPainterPath mShape;
};

#endif // LINKGRAPHICSITEM_H
