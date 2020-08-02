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
#include <VDIL/core/BlockInput.h>
#include <limits>

#define M_2PI 6.28318530717958647692

static const double arrAngle=M_PI/9.0;
static const double arrLen=15;
static const double arrowWidth=arrLen*sin(arrAngle)+1;
static const double linePointRad=2;
static const double linePointClickRadSq=10*10;

using namespace WLIOT;
using namespace WLIOTVDIL;

static double vDistSq(const QPointF &v)
{
	return v.x()*v.x()+v.y()*v.y();
}

static double vDot(const QPointF &v1,const QPointF &v2)
{
	return v1.x()*v2.x()+v1.y()*v2.y();
}

static double distToLineSq(const QPointF &p1,const QPointF &p2)//from (0,0) to line
{
	double d=p1.x()*p2.y()-p2.x()*p1.y();
	d*=d;
	d/=vDistSq(p2-p1);
	return d;
}

LinkGraphicsItem::LinkGraphicsItem(BlockGraphicsItemPort *from,BlockGraphicsItemPort *to,EditorInternalApi *ed)
{
	editor=ed;
	mFromPort=from;
	mToPort=to;
	calcCoordinates();
	dragPointIndex=-1;
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
	/*painter->drawLine(arrStartDrawPoint,arrEndDrawPoint);
	QPointF arr1(arrEndDrawPoint.x()-arrLen*cos(angle-arrAngle),arrEndDrawPoint.y()-arrLen*sin(angle-arrAngle));
	QPointF arr2(arrEndDrawPoint.x()-arrLen*cos(angle+arrAngle),arrEndDrawPoint.y()-arrLen*sin(angle+arrAngle));
	painter->drawLine(arrEndDrawPoint,arr1);
	painter->drawLine(arrEndDrawPoint,arr2);*/
	painter->drawPath(line);
	painter->setPen(QPen(Qt::red,3));
	painter->setBrush(Qt::red);
}

void LinkGraphicsItem::calcCoordinates()
{
	prepareGeometryChange();
	setPos(0,0);
	linePoints.clear();
	if(!mFromPort)
		linePoints.append(staticPos);
	else linePoints.append(mFromPort->mapToScene(mFromPort->center));
	if(!mToPort)
		linePoints.append(staticPos);
	else
	{
		linePoints.append(((BlockInput*)mToPort->port())->linePoints());
		linePoints.append(mToPort->mapToScene(mToPort->center));
	}
	if(linePoints.count()<2)
		return;//paranoid mode

	double rad=BlockGraphicsItemPort::portSize/2.0;

	//move first point from port center
	QPointF firstLine(linePoints[1].x()-linePoints[0].x(),linePoints[1].y()-linePoints[1].y());
	double firstLen=sqrt(firstLine.x()*firstLine.x()+firstLine.y()*firstLine.y());
	double firstAngle=0;
	if(fabs(firstLen)>0.0001)
		firstAngle=acos(firstLine.x()/firstLen);
	if(firstLine.y()<0)
		firstAngle=M_2PI-firstAngle;
	double cosA=cos(firstAngle);
	double sinA=sin(firstAngle);
	linePoints[0]+=QPointF(rad*cosA,rad*sinA);

	//move first point from port center
	int c=linePoints.count();
	QPointF lastLine(linePoints[c-1].x()-linePoints[c-2].x(),linePoints[c-1].y()-linePoints[c-2].y());
	double lastLen=sqrt(lastLine.x()*lastLine.x()+lastLine.y()*lastLine.y());
	double lastAngle=0;
	if(fabs(lastLen)>0.0001)
		lastAngle=acos(lastLine.x()/lastLen);
	if(lastLine.y()<0)
		lastAngle=M_2PI-lastAngle;
	cosA=cos(lastAngle);
	sinA=sin(lastAngle);
	linePoints[c-1]-=QPointF(rad*cosA,rad*sinA);

	//arrow lines
	QPointF lastPoint=linePoints[c-1];
	arrowEnd1Point=QPointF(lastPoint.x()-arrLen*cos(lastAngle-arrAngle),lastPoint.y()-arrLen*sin(lastAngle-arrAngle));
	arrowEnd2Point=QPointF(lastPoint.x()-arrLen*cos(lastAngle+arrAngle),lastPoint.y()-arrLen*sin(lastAngle+arrAngle));

//	QPointF topLeft(qMin(0.0,line.x()),qMin(0.0,line.y()));
//	QPointF bottomRight(qMax(0.0,line.x()),qMax(0.0,line.y()));
	line=QPainterPath();
	line.moveTo(linePoints[0]);
	for(int i=1;i<linePoints.count();++i)
		line.lineTo(linePoints[i]);
	line.lineTo(arrowEnd1Point);
	line.moveTo(linePoints.last());
	line.lineTo(arrowEnd2Point);
	for(int i=1;i<linePoints.count()-1;++i)
		line.addEllipse(linePoints[i],linePointRad,linePointRad);
	bRect=line.boundingRect();

	QPainterPathStroker t;
	t.setWidth(8);
	t.setCapStyle(Qt::FlatCap);
	t.setJoinStyle(Qt::BevelJoin);
	t.setDashPattern(Qt::SolidLine);
	mShape=t.createStroke(line);
//	double dx=arrowWidth*sinA;
//	double dy=arrowWidth*cosA;
//	mShape=QPainterPath();
//	mShape.setFillRule(Qt::WindingFill);
//	mShape.moveTo(-dx,+dy);
//	mShape.lineTo(line.x()-dx,line.y()+dy);
//	mShape.lineTo(line.x()+dx,line.y()-dy);
//	mShape.lineTo(dx,-dy);
//	mShape.closeSubpath();
}

BlockGraphicsItemPort* LinkGraphicsItem::from()
{
	return mFromPort;
}

BlockGraphicsItemPort* LinkGraphicsItem::to()
{
	return mToPort;
}

void LinkGraphicsItem::calcLinePointIndex(const QPointF &p,int &index,bool &existingPoint)
{
	for(int i=1;i<linePoints.count()-1;++i)//all but ports
	{
		if(vDistSq(p-linePoints[i])<linePointClickRadSq)
		{
			index=i;
			existingPoint=true;
			return;
		}
	}
	index=-1;
	existingPoint=false;
	double minDist=std::numeric_limits<double>::max();
	for(int i=0;i<linePoints.count()-1;++i)
	{
		QPointF p1=linePoints[i]-p;
		QPointF p2=linePoints[i+1]-p;
		QPointF pi=p2-p1;
		double d=0;
		double d1=-vDot(pi,p1);
		double d2=-vDot(pi,p2);
		if(d1<=0)
			d=vDistSq(p1);
		else if(d2>=0)
			d=vDistSq(p2);
		else d=distToLineSq(p1,p2);
		if(d<minDist)
		{
			minDist=d;
			index=i;
		}
	}
}

void LinkGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mousePressEvent(event);
	if(event->button()==Qt::LeftButton)
	{
		event->accept();
		if(!mToPort||!mFromPort)return;
		bool existingPoint=false;
		calcLinePointIndex(mapToScene(event->pos()),dragPointIndex,existingPoint);
		if(!existingPoint)
		{
			BlockInput *in=(BlockInput*)mToPort->port();
			in->addLinePoint(dragPointIndex,mapToScene(event->pos()));
			calcCoordinates();
		}
		else --dragPointIndex;
	}
	else if(event->button()==Qt::RightButton)
	{
		event->accept();
		editor->onLinkRClicked(this);
	}
}

void LinkGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mouseReleaseEvent(event);
	if(event->button()==Qt::LeftButton)
	{
		event->accept();
		if(dragPointIndex!=-1)
		{
			moveDragPoint(mapToScene(event->pos()));
			dragPointIndex=-1;
		}
	}
}

void LinkGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mouseMoveEvent(event);
	if(dragPointIndex!=-1)
		moveDragPoint(mapToScene(event->pos()));
}

void LinkGraphicsItem::moveDragPoint(const QPointF &scenePos)
{
	BlockInput *in=(BlockInput*)mToPort->port();
	if(vDistSq(scenePos-linePoints[dragPointIndex])<=linePointClickRadSq)
	{
		in->rmLinePoint(dragPointIndex);
		dragPointIndex=-1;
	}
	else if(dragPointIndex<(linePoints.count()-2)&&
		vDistSq(linePoints[dragPointIndex+2]-scenePos)<=linePointClickRadSq)
	{
		in->rmLinePoint(dragPointIndex);
		dragPointIndex=-1;
	}
	else in->setLinePoint(dragPointIndex,scenePos);
	calcCoordinates();
}
