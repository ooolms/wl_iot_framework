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

#ifndef BLOCKGRAPHICSITEM_H
#define BLOCKGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsPixmapItem>
#include "GDIL/core/BaseBlock.h"
#include "BlockGraphicsItemPort.h"
#include "BlockGraphicsItemConfigButton.h"
#include "LinkGraphicsItem.h"

class EditorInternalApi;
class BlockGraphicsItemHeader;

class BlockGraphicsItem
	:public QGraphicsItem
{
public:
	explicit BlockGraphicsItem(BaseBlock *block,EditorInternalApi *e,const QString &blockTypeStr);
	~BlockGraphicsItem();
	virtual QRectF boundingRect()const override;
	virtual void paint(QPainter *painter,const QStyleOptionGraphicsItem *,QWidget *)override;
	void createLinks();
	QPointF inputCenter(int index);
	QPointF outputCenter(int index);
	static void setTextFitToWidth(QGraphicsSimpleTextItem *textItem,double w,const QString &text);
	void updateLinkPositions();
	BaseBlock *block();

private:
	void onPortLClicked(BlockGraphicsItemPort *port);
	void onHeaderLClicked();
	void onHeaderRClicked();
	void onHeaderReleased();
	void onHeaderMovedBy(QPointF dist);
	void onSettingsClicked();

private:
	BaseBlock *mBlock;
	EditorInternalApi *editor;
	QGraphicsSimpleTextItem *hintItem,*typeItem;
	BlockGraphicsItemHeader *titleItem;
	BlockGraphicsItemConfigButton *settingsBtnItem;
	QList<BlockGraphicsItemPort*> inputPorts,outputPorts;
	QList<LinkGraphicsItem*> inputLinks;
	QList<LinkGraphicsItem*> outputLinks;
	QRectF bRect;
	QString mBlockType;
	friend class BlockGraphicsItemPort;
	friend class BlockGraphicsItemHeader;
	friend class BlockGraphicsItemConfigButton;
};

#endif // BLOCKGRAPHICSITEM_H
