#ifndef BLOCKGRAPHICSITEMPORT_H
#define BLOCKGRAPHICSITEMPORT_H

#include "GDIL/core/BlockPort.h"
#include <QGraphicsItem>
#include <QGraphicsSimpleTextItem>

class BlockGraphicsItem;

class BlockGraphicsItemPort
	:public QGraphicsItem
{
public:
	explicit BlockGraphicsItemPort(BlockGraphicsItem *blockItem,BlockPort *port,bool input,int portIndex);
	virtual QRectF boundingRect()const override;
	virtual void paint(QPainter *painter,const QStyleOptionGraphicsItem *,QWidget *)override;
	BlockPort* port();

public:
	static const double portSize;
	static const QPointF center;

private:
	BlockGraphicsItem *mBlockItem;
	QGraphicsSimpleTextItem *dimText;
	BlockPort *mPort;
	bool mIsInputPort;
	int mPortIndex;
	static const QRectF bRect;
};

#endif // BLOCKGRAPHICSITEMPORT_H
