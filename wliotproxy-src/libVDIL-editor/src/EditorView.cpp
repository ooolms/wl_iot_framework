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

#include "EditorView.h"
#include <QMouseEvent>
#include <QScrollBar>
#include <QApplication>

using namespace WLIOTVDIL;

EditorView::EditorView(QWidget *parent)
	:QGraphicsView(parent)
{
	dragScene=false;
}

void EditorView::mousePressEvent(QMouseEvent *event)
{
	if(event->button()!=Qt::LeftButton)
	{
		QGraphicsView::mousePressEvent(event);
		return;
	}
	QGraphicsScene *sc=scene();
	event->accept();
	if(!sc)return;
	QPointF scPt=mapToScene(event->pos());
	if(sc->itemAt(scPt,QTransform()))
	{
		QGraphicsView::mousePressEvent(event);
		return;
	}
	dragScene=true;
	startDragPoint=prevDragPoint=event->pos();
}

void EditorView::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button()!=Qt::LeftButton)
		QGraphicsView::mouseReleaseEvent(event);
	else if(dragScene)
	{
		onDragScene(event->pos());
		dragScene=false;
		if((event->pos()-startDragPoint).manhattanLength()<QApplication::startDragDistance())
		{
			QScopedPointer<QMouseEvent> virtualPressEvent(new QMouseEvent(QEvent::MouseButtonPress,startDragPoint,
				event->button(),event->buttons()|event->button(),event->modifiers()));
			QGraphicsView::mousePressEvent(virtualPressEvent.data());
			QGraphicsView::mouseReleaseEvent(event);
		}
	}
	else QGraphicsView::mouseReleaseEvent(event);
}

void EditorView::mouseMoveEvent(QMouseEvent *event)
{
	if(dragScene)
	{
		if((event->pos()-startDragPoint).manhattanLength()>=QApplication::startDragDistance())
			onDragScene(event->pos());
	}
	else QGraphicsView::mouseMoveEvent(event);
}

void EditorView::onDragScene(const QPoint &newPt)
{
	QPoint delta=newPt-prevDragPoint;
	prevDragPoint=newPt;
	verticalScrollBar()->setValue(verticalScrollBar()->value()-delta.y());
	horizontalScrollBar()->setValue(horizontalScrollBar()->value()+delta.x());
}
