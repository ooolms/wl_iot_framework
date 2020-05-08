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

#ifndef EDITOR_H
#define EDITOR_H

#include <QWidget>
#include <QMap>
#include <QTreeWidget>
#include "GDIL/core/BlocksFactory.h"
#include "GDIL/editor/BlocksEditingFactory.h"

class Program;
class ProgramObject;
class BaseBlock;
class BlocksXmlParserFactory;
class BlockGraphicsItem;
class BlockGraphicsItemPort;
class EditorInternalApi;
class LinkGraphicsItem;
class EditorScene;
class QGraphicsView;
class QComboBox;
class QPushButton;

class Editor
	:public QWidget
{
	Q_OBJECT
public:
	explicit Editor(BlocksFactory *blocksFact,BlocksXmlParserFactory *blocksXmlFact,
		BlocksEditingFactory *blocksEdFact,QWidget *parent=nullptr);
	virtual ~Editor();
	bool setProgram(const QByteArray &xmlData);
	QByteArray getProgram();
	virtual bool eventFilter(QObject *watched, QEvent *event) override;
	void setEngineHelper(IEngineHelper *hlp);
	void setEngineCallbacks(IEngineCallbacks *cb);

signals:
	void selectDevice(QUuid &deviceId,QString &deviceName,ControlsGroup &controls);
	void selectStorage(StorageId &storId,QString &deviceName,SensorDef::Type &valuesType);
	void execCommand(const QUuid &devId,const QByteArray &cmd,const QByteArrayList &args);
	void debugMessage(const QString &m);

private slots:
	void onBlocksToolbarSelChanged();
	void onBlocksGroupSelected(int index);
	void onEditTriggersClicked();

private:
	void renderProgram();
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
	void onHeaderRClicked(BlockGraphicsItem *item);
	void onHeaderReleased(BlockGraphicsItem *item);
	void onHeaderMovedBy(BlockGraphicsItem *item,QPointF dist);
	bool editBlockSettings(BaseBlock *b);

private:
	EditorInternalApi *edApi;
	Program *prg;
	ProgramObject *prgObj;
	QMap<BlockGraphicsItem*,BaseBlock*> itemToBlockMap;
	QMap<BaseBlock*,BlockGraphicsItem*> blockToItemMap;
	BlocksFactory *mBlocksFactory;
	BlocksXmlParserFactory *mBlocksXmlParserFactory;
	BlocksEditingFactory *mBlocksEditingFactory;
	LinkGraphicsItem *drawTmpLink;
	EditorScene *scene;
	QGraphicsView *view;
	QComboBox *blocksGroupSelect;
	QTreeWidget *blocksToolbar;
	QPushButton *editTriggersBtn;
	QMap<QTreeWidgetItem*,QString> toolbarActionToTypeMap;
	QMap<QString,QTreeWidgetItem*> toolbarTypeToActionMap;
	QString currentBlocksGroup;
	QString currentPlacedBlockName;
	QCursor aimCursor;
	IEngineHelper *helper;
	IEngineCallbacks *callbacks;
	friend class EditorInternalApi;
};

#endif // EDITOR_H