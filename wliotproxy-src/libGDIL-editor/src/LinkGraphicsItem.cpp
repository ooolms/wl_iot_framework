#include "GDIL/editor/LinkGraphicsItem.h"
#include "GDIL/editor/BlockGraphicsItemPort.h"
#include <math.h>
#include <QPainter>

#define M_2PI 6.28318530717959

static const double arrAngle=M_PI/9.0;
static const double arrLen=15;
static const double arrowWidth=arrLen*sin(arrAngle)+1;

LinkGraphicsItem::LinkGraphicsItem(BlockGraphicsItemPort *from,BlockGraphicsItemPort *to)
{
	mFromPort=from;
	mToPort=to;
	mFrom=from->mapToScene(from->center);
	mTo=to->mapToScene(to->center);
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
	arrStartDrawPoint.setX(mFrom.x()+rad*cosA);
	arrStartDrawPoint.setY(mFrom.y()+rad*sinA);
	arrEndDrawPoint.setX(mTo.x()-rad*cosA);
	arrEndDrawPoint.setY(mTo.y()-rad*sinA);

	bRect=QRectF(mFrom,mTo);

	double dx=arrowWidth*sinA;
	double dy=arrowWidth*cosA;
	mShape.setFillRule(Qt::WindingFill);
	mShape.moveTo(mFrom.x()-dx,mFrom.y()+dy);
	mShape.lineTo(mTo.x()-dx,mTo.y()+dy);
	mShape.lineTo(mTo.x()+dx,mTo.y()-dy);
	mShape.lineTo(mFrom.x()+dx,mFrom.y()-dy);
	mShape.closeSubpath();
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
