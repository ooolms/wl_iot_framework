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

#ifndef EDITORTAB_H
#define EDITORTAB_H

#include <QWidget>
#include <QMap>
#include <QTreeWidget>
#include "VDIL/core/BlocksFactory.h"
#include "VDIL/editor/BlocksEditingFactory.h"
#include "VDIL/editor/IEditorHelper.h"

class QGraphicsView;
class QComboBox;
class QPushButton;

namespace WLIOTVDIL
{
	class ProgramObject;
	class BaseBlock;
	class BlocksXmlParserFactory;
	class BlockGraphicsItem;
	class BlockGraphicsItemPort;
	class EditorInternalApi;
	class LinkGraphicsItem;
	class EditorScene;
	class SubProgramBlock;
	class Editor;

	class EditorTab
		:public QWidget
	{
		Q_OBJECT
	public:
		explicit EditorTab(Editor *ed,SubProgram *p,SubProgramBlock *b,QWidget *parent=nullptr);
		virtual ~EditorTab();
		void renderProgram();

	private:
		void onPortLClicked(BlockGraphicsItemPort *port);
		void onLinkRClicked(LinkGraphicsItem *link);
		void onSceneLClicked(QPointF pos);
		void onSceneLReleased(QPointF pos);
		void onSceneRClicked(QPointF pos);
		void onSceneMouseMove(QPointF pos);
		void onBlockLClicked(BlockGraphicsItem *item);
		void onBlockRClicked(BlockGraphicsItem *item);
		void onBlockSettingsClicked(BlockGraphicsItem *item);
		void onHeaderLClicked(BlockGraphicsItem *item);
		void onHeaderDClicked(BlockGraphicsItem *item);
		void onHeaderRClicked(BlockGraphicsItem *item);
		void onHeaderReleased(BlockGraphicsItem *item);
		void onHeaderMovedBy(BlockGraphicsItem *item,QPointF dist);
		bool editBlockSettings(BaseBlock *b);
		void rmBlock(BlockGraphicsItem *item);

	private:
		Editor *mEd;
		SubProgramBlock *mSubProgramBlock;
		EditorInternalApi *edApi;
		SubProgram *prg;
		QMap<BlockGraphicsItem*,BaseBlock*> itemToBlockMap;
		QMap<BaseBlock*,BlockGraphicsItem*> blockToItemMap;
		LinkGraphicsItem *drawTmpLink;
		EditorScene *scene;
		QGraphicsView *view;
		QMap<QTreeWidgetItem*,QString> toolbarActionToTypeMap;
		QMap<QString,QTreeWidgetItem*> toolbarTypeToActionMap;
		QCursor aimCursor;
		friend class EditorInternalApi;
	};
}

#endif // EDITORTAB_H
