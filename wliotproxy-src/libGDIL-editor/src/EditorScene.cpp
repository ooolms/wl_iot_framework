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

#include "EditorScene.h"
#include "EditorInternalApi.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

using namespace WLIOT;
using namespace WLIOTGDIL;

EditorScene::EditorScene(EditorInternalApi *ed,QObject *parent)
	:QGraphicsScene(parent)
{
	editor=ed;
}

void EditorScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsScene::mousePressEvent(event);
	if(event->isAccepted())return;
	if(event->button()==Qt::LeftButton)
		editor->onSceneLClicked(event->scenePos());
	else if(event->button()==Qt::RightButton)
		editor->onSceneRClicked(event->scenePos());
}

void EditorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsScene::mouseReleaseEvent(event);
	editor->onSceneLReleased(event->scenePos());
}

void EditorScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsScene::mouseMoveEvent(event);
	editor->onSceneMouseMove(event->scenePos());
}
