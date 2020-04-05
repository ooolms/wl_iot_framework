#include "GDIL/editor/Editor.h"
#include "GDIL/core/Program.h"
#include "GDIL/xml/ProgramXmlParser.h"
#include "GDIL/xml/BlocksXmlParserFactory.h"
#include "GDIL/core/BlocksFactory.h"
#include "GDIL/editor/BlockGraphicsItem.h"
#include <QLayout>

Editor::Editor(BlocksFactory *blocksFact,BlocksXmlParserFactory *blocksXmlFact,QWidget *parent)
	:QWidget(parent)
{
	prg=new Program;
	mBlocksFactory=blocksFact;
	mBlocksXmlParserFactory=blocksXmlFact;
	scene=new QGraphicsScene(this);
	view=new QGraphicsView(this);
	(new QVBoxLayout(this))->addWidget(view);
	view->setScene(scene);
}

Editor::~Editor()
{
	delete prg;
}

bool Editor::setProgram(const QByteArray &xmlData)
{
	Program *p=ProgramXmlParser::fromXml(mBlocksXmlParserFactory,mBlocksFactory,xmlData);
	if(!p)return false;
	delete prg;
	prg=p;
	renderProgram();
	return true;
}

QByteArray Editor::getProgram()
{
	return ProgramXmlParser::toXml(mBlocksXmlParserFactory,prg);
}

void Editor::renderProgram()
{
	for(auto i=blockToItemMap.begin();i!=blockToItemMap.end();++i)
		delete i.value();
	blockToItemMap.clear();
	itemToBlockMap.clear();
	for(auto i=prg->sourceBlocks().begin();i!=prg->sourceBlocks().end();++i)
	{
		BaseBlock *b=i.value();
		BlockGraphicsItem *item=new BlockGraphicsItem(b,this);
		blockToItemMap[b]=item;
		itemToBlockMap[item]=b;
		scene->addItem(item);
	}
	for(auto i=prg->processingBlocks().begin();i!=prg->processingBlocks().end();++i)
	{
		BaseBlock *b=i.value();
		BlockGraphicsItem *item=new BlockGraphicsItem(b,this);
		blockToItemMap[b]=item;
		itemToBlockMap[item]=b;
		scene->addItem(item);
	}
	for(auto i=blockToItemMap.begin();i!=blockToItemMap.end();++i)
		i.value()->createLinks();
}
