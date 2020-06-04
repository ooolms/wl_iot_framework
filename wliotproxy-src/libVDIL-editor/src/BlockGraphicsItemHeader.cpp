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

#include "BlockGraphicsItemHeader.h"
#include "BlockGraphicsItem.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

using namespace WLIOT;
using namespace WLIOTVDIL;

BlockGraphicsItemHeader::BlockGraphicsItemHeader(BlockGraphicsItem *blockItem)
	:QGraphicsSimpleTextItem(blockItem)
{
	item=blockItem;
	dragging=false;
	QFont f=font();
	f.setBold(true);
	f.setUnderline(true);
	setFont(f);
}

void BlockGraphicsItemHeader::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsSimpleTextItem::mousePressEvent(event);
	if(event->button()==Qt::LeftButton)
	{
		event->accept();
		dragging=true;
		prevDragPos=mapToScene(event->pos());
		item->onHeaderLClicked();
	}
	else if(event->button()==Qt::RightButton&&!dragging)
	{
		event->accept();
		item->onHeaderRClicked();
	}
}

void BlockGraphicsItemHeader::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsSimpleTextItem::mouseReleaseEvent(event);
	if(dragging)
	{
		dragging=false;
		item->onHeaderMovedBy(mapToScene(event->pos())-prevDragPos);
		item->onHeaderReleased();
	}
}

void BlockGraphicsItemHeader::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsSimpleTextItem::mouseMoveEvent(event);
	if(!dragging)return;
	QPointF nextPos=mapToScene(event->pos());
	QPointF dist=nextPos-prevDragPos;
	prevDragPos=nextPos;
	item->onHeaderMovedBy(dist);
}
