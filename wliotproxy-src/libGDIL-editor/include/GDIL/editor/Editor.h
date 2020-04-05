#ifndef EDITOR_H
#define EDITOR_H

#include <QWidget>
#include <QMap>
#include <QGraphicsScene>
#include <QGraphicsView>

class Program;
class BaseBlock;
class BlocksFactory;
class BlocksXmlParserFactory;
class BlockGraphicsItem;

class Editor
	:public QWidget
{
	Q_OBJECT
public:
	explicit Editor(BlocksFactory *blocksFact,BlocksXmlParserFactory *blocksXmlFact,QWidget *parent=nullptr);
	virtual ~Editor();
	bool setProgram(const QByteArray &xmlData);
	QByteArray getProgram();

private:
	void renderProgram();

private:
	Program *prg;
	QMap<BlockGraphicsItem*,BaseBlock*> itemToBlockMap;
	QMap<BaseBlock*,BlockGraphicsItem*> blockToItemMap;
	BlocksFactory *mBlocksFactory;
	BlocksXmlParserFactory *mBlocksXmlParserFactory;
	QGraphicsScene *scene;
	QGraphicsView *view;
	friend class BlockGraphicsItem;
};

#endif // EDITOR_H
