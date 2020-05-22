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

#include "EditorInternalApi.h"
#include "GDIL/editor/Editor.h"

using namespace WLIOT;
using namespace WLIOTGDIL;

EditorInternalApi::EditorInternalApi(Editor *e)
{
	ed=e;
}

void EditorInternalApi::onPortLClicked(BlockGraphicsItemPort *port)
{
	ed->onPortLClicked(port);
}

void EditorInternalApi::onLinkRClicked(LinkGraphicsItem *link)
{
	ed->onLinkRClicked(link);
}

void EditorInternalApi::onSceneLClicked(QPointF pos)
{
	ed->onSceneLClicked(pos);
}

void EditorInternalApi::onSceneLReleased(QPointF pos)
{
	ed->onSceneLReleased(pos);
}

void EditorInternalApi::onSceneRClicked(QPointF pos)
{
	ed->onSceneRClicked(pos);
}

void EditorInternalApi::onSceneMouseMove(QPointF pos)
{
	ed->onSceneMouseMove(pos);
}

void EditorInternalApi::onBlockLClicked(BlockGraphicsItem *item)
{
	ed->onBlockLClicked(item);
}

void EditorInternalApi::onBlockRClicked(BlockGraphicsItem *item)
{
	ed->onBlockRClicked(item);
}

void EditorInternalApi::onBlockSettingsClicked(BlockGraphicsItem *item)
{
	ed->onBlockSettingsClicked(item);
}

void EditorInternalApi::onHeaderLClicked(BlockGraphicsItem *item)
{
	ed->onHeaderLClicked(item);
}

void EditorInternalApi::onHeaderRClicked(BlockGraphicsItem *item)
{
	ed->onHeaderRClicked(item);
}

void EditorInternalApi::onHeaderReleased(BlockGraphicsItem *item)
{
	ed->onHeaderReleased(item);
}

void EditorInternalApi::onHeaderMovedBy(BlockGraphicsItem *item, QPointF dist)
{
	ed->onHeaderMovedBy(item,dist);
}

Editor* EditorInternalApi::editor()
{
	return ed;
}

QMap<BlockGraphicsItem*,BaseBlock*>& EditorInternalApi::itemToBlockMap()
{
	return ed->itemToBlockMap;
}

QMap<BaseBlock*,BlockGraphicsItem*>& EditorInternalApi::blockToItemMap()
{
	return ed->blockToItemMap;
}

EditorScene* EditorInternalApi::scene()
{
	return ed->scene;
}

BlocksFactory* EditorInternalApi::blocksFactory()
{
	return ed->mBlocksFactory;
}

QString EditorInternalApi::blockHint(BaseBlock *b)
{
	IBlockEditor *e=ed->mBlocksEditingFactory->editor(b->groupName(),b->blockName());
	if(!e)return QString();
	return e->hint(ed->editorHelper,b);
}
