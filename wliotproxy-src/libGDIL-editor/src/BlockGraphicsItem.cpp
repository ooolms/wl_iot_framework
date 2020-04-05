#include "GDIL/editor/BlockGraphicsItem.h"
#include "GDIL/editor/Editor.h"
#include <QFont>
#include <QPainter>

static const double minTitleWidth=80;
static const double blockMargin=10;
static const double portHeight=20;

BlockGraphicsItem::BlockGraphicsItem(BaseBlock *block,Editor *e)
{
	mBlock=block;
	editor=e;

	titleItem=new QGraphicsSimpleTextItem(this);
	titleItem->setPos(blockMargin,blockMargin);
	QFont f=titleItem->font();
	f.setBold(true);
	titleItem->setFont(f);
	titleItem->setText(mBlock->title);

	double hintOffset=2*blockMargin+titleItem->boundingRect().height();
	hintItem=new QGraphicsSimpleTextItem(this);
	hintItem->setToolTip(mBlock->hint);
	hintItem->setPos(blockMargin,hintOffset);

	double titleWidth=titleItem->boundingRect().width();
	if(titleWidth<minTitleWidth)
		titleWidth=minTitleWidth;
	double totalWidth=titleWidth+blockMargin*2;

	hintItem->setText("W");
	double portUnitHeight=qMax(hintItem->boundingRect().height(),portHeight);
	setTextFitToWidth(hintItem,titleWidth,mBlock->hint.trimmed());

	double inputsOffset=hintOffset+blockMargin+hintItem->boundingRect().height();
	double outputsOffset=inputsOffset+portUnitHeight+blockMargin;
	double portsDist=2*(portUnitHeight+blockMargin);

	for(int i=0;i<block->inputsCount();++i)
	{
		BlockInput *in=block->input(i);
		BlockGraphicsItemPort *port=new BlockGraphicsItemPort(this,in,true,i);
		QGraphicsSimpleTextItem *portText=new QGraphicsSimpleTextItem(this);
		setTextFitToWidth(portText,titleWidth-BlockGraphicsItemPort::portSize,in->title());
		port->setPos(0,inputsOffset+portsDist*i);
		portText->setPos(BlockGraphicsItemPort::portSize+blockMargin,inputsOffset+portsDist*i);
		inputPorts.append(port);
	}

	for(int i=0;i<block->outputsCount();++i)
	{
		BlockOutput *out=block->output(i);
		BlockGraphicsItemPort *port=new BlockGraphicsItemPort(this,block->output(i),false,i);
		QGraphicsSimpleTextItem *portText=new QGraphicsSimpleTextItem(this);
		setTextFitToWidth(portText,titleWidth-BlockGraphicsItemPort::portSize,out->title());
		port->setPos(totalWidth-BlockGraphicsItemPort::portSize,outputsOffset+portsDist*i);
		portText->setPos(blockMargin,outputsOffset+portsDist*i);
		outputPorts.append(port);
	}

	bRect.setWidth(totalWidth);
	if(block->inputsCount()!=0||block->outputsCount()!=0)
	{
		bRect.setHeight(qMax(inputsOffset+portsDist*block->inputsCount()-portsDist/2.0,
			outputsOffset+portsDist*block->outputsCount()-portsDist/2.0));
	}
	else bRect.setHeight(inputsOffset);
	setPos(block->position);
}

BlockGraphicsItem::~BlockGraphicsItem()
{
	for(int i=0;i<inputLinks.count();++i)
		delete inputLinks[i];
}

QRectF BlockGraphicsItem::boundingRect()const
{
	return bRect;
}

void BlockGraphicsItem::paint(QPainter *painter,const QStyleOptionGraphicsItem *,QWidget *)
{
	painter->setPen(QPen(Qt::black,3));
	painter->drawRect(bRect);
}

void BlockGraphicsItem::createLinks()
{
	for(int i=0;i<inputLinks.count();++i)
		delete inputLinks[i];
	inputLinks.clear();
	for(int i=0;i<inputPorts.count();++i)
	{
		BlockGraphicsItemPort *toPort=inputPorts[i];
		BlockInput *in=(BlockInput*)toPort->port();
		if(!in->linkedOutput())continue;
		BaseBlock *linkedBlock=in->linkedOutput()->block();
		BlockGraphicsItem *linkedItem=editor->blockToItemMap.value(linkedBlock);
		if(!linkedItem)continue;

		int outputIndex=linkedBlock->outputIndex(in->linkedOutput());
		BlockGraphicsItemPort *fromPort=linkedItem->outputPorts.value(outputIndex);
		if(!fromPort)continue;
		LinkGraphicsItem *linkItem=new LinkGraphicsItem(fromPort,toPort);
		inputLinks.append(linkItem);
		scene()->addItem(linkItem);
	}
}

QPointF BlockGraphicsItem::inputCenter(int index)
{
	if(index<0||index>=inputPorts.count())
		return QPointF(0,0);
	return inputPorts[index]->mapToScene(BlockGraphicsItemPort::center);
}

QPointF BlockGraphicsItem::outputCenter(int index)
{
	if(index<0||index>=outputPorts.count())
		return QPointF(0,0);
	return outputPorts[index]->mapToScene(BlockGraphicsItemPort::center);
}

void BlockGraphicsItem::setTextFitToWidth(QGraphicsSimpleTextItem *textItem,double w,const QString &text)
{
	QString textCopy=text;
	bool choped=false;
	textItem->setText(textCopy);
	if(textCopy.contains('\n'))
	{
		textCopy=textCopy.left(textCopy.indexOf('\n'));
		textItem->setText(textCopy+"...");
		choped=true;
	}
	while(textItem->boundingRect().width()>w&&!textCopy.isEmpty())
	{
		textCopy.chop(1);
		textItem->setText(textCopy+"...");
		choped=true;
	}
	if(choped)
		textItem->setToolTip(text);
}
