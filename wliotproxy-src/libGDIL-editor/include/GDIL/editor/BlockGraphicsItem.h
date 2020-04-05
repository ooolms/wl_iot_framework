#ifndef BLOCKGRAPHICSITEM_H
#define BLOCKGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QGraphicsSimpleTextItem>
#include "GDIL/core/BaseBlock.h"
#include "GDIL/editor/BlockGraphicsItemPort.h"
#include "GDIL/editor/LinkGraphicsItem.h"

class Editor;

class BlockGraphicsItem
	:public QGraphicsItem
{
public:
	explicit BlockGraphicsItem(BaseBlock *block,Editor *e);
	~BlockGraphicsItem();
	virtual QRectF boundingRect()const override;
	virtual void paint(QPainter *painter,const QStyleOptionGraphicsItem *,QWidget *)override;
	void createLinks();
	QPointF inputCenter(int index);
	QPointF outputCenter(int index);

public:
	static void setTextFitToWidth(QGraphicsSimpleTextItem *textItem,double w,const QString &text);

private:
	BaseBlock *mBlock;
	Editor *editor;
	QGraphicsSimpleTextItem *titleItem,*hintItem;
	QList<BlockGraphicsItemPort*> inputPorts,outputPorts;
	QList<LinkGraphicsItem*> inputLinks;
	QRectF bRect;
};

#endif // BLOCKGRAPHICSITEM_H
