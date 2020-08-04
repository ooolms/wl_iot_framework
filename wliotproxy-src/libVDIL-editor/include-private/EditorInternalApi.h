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

#ifndef EDITORINTERNALAPI_H
#define EDITORINTERNALAPI_H

#include <QMap>
#include <QPointF>
#include <QUuid>
#include "wliot/devices/SensorDef.h"
#include "wliot/storages/StorageId.h"
#include "wliot/devices/ControlsDefinition.h"
#include "VDIL/editor/IEditorHelper.h"

class QGraphicsScene;

namespace WLIOTVDIL
{
	class EditorTab;
	class EditorScene;
	class BlockGraphicsItem;
	class BlockGraphicsItemPort;
	class LinkGraphicsItem;
	class BaseBlock;
	class BlocksFactory;
	class BlocksXmlParserFactory;

	class EditorInternalApi
	{
	public:
		explicit EditorInternalApi(EditorTab *e);
		void onPortLClicked(BlockGraphicsItemPort *port);
		void onLinkRClicked(LinkGraphicsItem *link);
		void onSceneLClicked(QPointF scenePos,QPoint screenPos);
		void onSceneLReleased(QPointF scenePos,QPoint screenPos);
		void onSceneRClicked(QPointF scenePos,QPoint screenPos);
		void onSceneMouseMove(QPointF scenePos,QPoint screenPos);
		void onBlockLClicked(BlockGraphicsItem *item);
		void onBlockRClicked(BlockGraphicsItem *item);
		void onBlockSettingsClicked(BlockGraphicsItem *item);
		void onHeaderLClicked(BlockGraphicsItem *item);
		void onHeaderDClicked(BlockGraphicsItem *item);
		void onHeaderRClicked(BlockGraphicsItem *item);
		void onHeaderReleased(BlockGraphicsItem *item);
		void onHeaderMovedBy(BlockGraphicsItem *item,QPointF dist);
		EditorTab* editor();
		BlockGraphicsItem* itemForBlock(BaseBlock *b);
		EditorScene *scene();
		QString blockHint(BaseBlock *b);

	private:
		EditorTab *ed;
	};
}

#endif // EDITORINTERNALAPI_H
