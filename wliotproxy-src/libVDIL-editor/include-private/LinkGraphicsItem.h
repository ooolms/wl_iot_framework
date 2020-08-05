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

#ifndef LINKGRAPHICSITEM_H
#define LINKGRAPHICSITEM_H

#include <QGraphicsItem>

namespace WLIOTVDIL
{
	class BlockGraphicsItemPort;
	class EditorInternalApi;

	class LinkGraphicsItem
		:public QGraphicsItem
	{
	public:
		explicit LinkGraphicsItem(BlockGraphicsItemPort *from,BlockGraphicsItemPort *to,EditorInternalApi *ed);
		void setStaticCoordinates(QPointF pos);//pos for a temporary arrow when drawing
		virtual QRectF boundingRect()const override;
		virtual QPainterPath shape()const override;
		virtual void paint(QPainter *painter,const QStyleOptionGraphicsItem *,QWidget *)override;
		void calcCoordinates();
		BlockGraphicsItemPort* from();
		BlockGraphicsItemPort* to();
		void calcLinePointIndex(const QPointF &p,int &index,bool &existingPoint);

	protected:
		virtual void mousePressEvent(QGraphicsSceneMouseEvent *event)override;
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event)override;
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event)override;

	private:
		void moveDragPoint(QPointF scenePos);

	public:
		EditorInternalApi *editor;
		BlockGraphicsItemPort *mFromPort,*mToPort;
		QPointF staticPos;
		QRectF bRect;
		QList<QPointF> linePoints;//all points with start port and end port
		QPainterPath line;
		QPolygonF arrow;
		QPainterPath mShape;
		int dragPointIndex;
	};
}

#endif // LINKGRAPHICSITEM_H
