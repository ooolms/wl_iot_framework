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

#ifndef BLOCKGRAPHICSITEMPORT_H
#define BLOCKGRAPHICSITEMPORT_H

#include "VDIL/core/BlockPort.h"
#include <QBrush>
#include <QGraphicsItem>
#include <QGraphicsSimpleTextItem>

namespace WLIOTVDIL
{
	class BlockGraphicsItem;

	class BlockGraphicsItemPort
		:public QGraphicsItem
	{
	public:
		explicit BlockGraphicsItemPort(BlockGraphicsItem *blockItem,BlockPort *port,bool input,int portIndex);
		virtual QRectF boundingRect()const override;
		virtual void paint(QPainter *painter,const QStyleOptionGraphicsItem *,QWidget *)override;
		BlockPort* port();
		BlockGraphicsItem* blockItem();
		bool isInput()const;

	protected:
		virtual void mousePressEvent(QGraphicsSceneMouseEvent *event)override;

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
		QBrush fillBrush;
	};
}

#endif // BLOCKGRAPHICSITEMPORT_H
