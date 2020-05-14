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

#include "LinkGraphicsItem.h"
#include "BlockGraphicsItemPort.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include "EditorInternalApi.h"
#include <QGraphicsScene>
#include <QDebug>
#include <qmath.h>

#define M_2PI 6.28318530717958647692

static const double arrAngle=M_PI/9.0;
static const double arrLen=15;
static const double arrowWidth=arrLen*sin(arrAngle)+1;

LinkGraphicsItem::LinkGraphicsItem(BlockGraphicsItemPort *from,BlockGraphicsItemPort *to,EditorInternalApi *ed)
{
	editor=ed;
	mFromPort=from;
	mToPort=to;
	calcCoordinates();
}

void LinkGraphicsItem::setStaticCoordinates(QPointF pos)
{
	staticPos=pos;
	calcCoordinates();
}

QRectF LinkGraphicsItem::boundingRect()const
{
	return bRect;
}

QPainterPath LinkGraphicsItem::shape()const
{
	return mShape;
}

void LinkGraphicsItem::paint(QPainter *painter,const QStyleOptionGraphicsItem *,QWidget *)
{
	painter->setRenderHint(QPainter::Antialiasing);
	painter->setPen(QPen(Qt::black,3));
	painter->drawLine(arrStartDrawPoint,arrEndDrawPoint);
	QPointF arr1(arrEndDrawPoint.x()-arrLen*cos(angle-arrAngle),arrEndDrawPoint.y()-arrLen*sin(angle-arrAngle));
	QPointF arr2(arrEndDrawPoint.x()-arrLen*cos(angle+arrAngle),arrEndDrawPoint.y()-arrLen*sin(angle+arrAngle));
	painter->drawLine(arrEndDrawPoint,arr1);
	painter->drawLine(arrEndDrawPoint,arr2);
}

void LinkGraphicsItem::calcCoordinates()
{
	prepareGeometryChange();
	if(mFromPort)
		mFrom=mFromPort->mapToScene(mFromPort->center);
	else mFrom=staticPos;
	if(mToPort)
		mTo=mToPort->mapToScene(mToPort->center);
	else mTo=staticPos;
	setPos(mFrom);
	QPointF line(mTo.x()-mFrom.x(),mTo.y()-mFrom.y());

	double len=sqrt(line.x()*line.x()+line.y()*line.y());
	if(fabs(len)>0.0001)
		angle=acos(line.x()/len);
	else angle=0;
	if(line.y()<0)
		angle=M_2PI-angle;
	double cosA=cos(angle);
	double sinA=sin(angle);
	double rad=BlockGraphicsItemPort::portSize/2.0;

	arrStartDrawPoint.setX(rad*cosA);
	arrStartDrawPoint.setY(rad*sinA);
	arrEndDrawPoint.setX(line.x()-rad*cosA);
	arrEndDrawPoint.setY(line.y()-rad*sinA);

	QPointF topLeft(qMin(0.0,line.x()),qMin(0.0,line.y()));
	QPointF bottomRight(qMax(0.0,line.x()),qMax(0.0,line.y()));
	bRect=QRectF(topLeft,bottomRight);

	double dx=arrowWidth*sinA;
	double dy=arrowWidth*cosA;
	mShape=QPainterPath();
	mShape.setFillRule(Qt::WindingFill);
	mShape.moveTo(-dx,+dy);
	mShape.lineTo(line.x()-dx,line.y()+dy);
	mShape.lineTo(line.x()+dx,line.y()-dy);
	mShape.lineTo(dx,-dy);
	mShape.closeSubpath();
}

BlockGraphicsItemPort* LinkGraphicsItem::from()
{
	return mFromPort;
}

BlockGraphicsItemPort* LinkGraphicsItem::to()
{
	return mToPort;
}

void LinkGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mousePressEvent(event);
	event->accept();
	if(event->button()==Qt::RightButton)
	{
		event->accept();
		editor->onLinkRClicked(this);
	}
}
