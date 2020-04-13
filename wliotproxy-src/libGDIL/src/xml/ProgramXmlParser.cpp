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

#include "GDIL/xml/ProgramXmlParser.h"
#include "GDIL/xml/BlocksXmlParserFactory.h"
#include "GDIL/core/BlocksFactory.h"
#include <QDomDocument>
#include <QDomElement>

QByteArray ProgramXmlParser::toXml(BlocksXmlParserFactory *f,const Program *p)
{
	QDomDocument doc;
	QDomElement root=doc.createElement("GDIL_program");
	doc.appendChild(root);
	QDomElement blocksElem=doc.createElement("blocks");
	root.appendChild(blocksElem);
	for(auto i=p->sourceBlocks().begin();i!=p->sourceBlocks().end();++i)
		if(!blockToXml(f,i.value(),blocksElem))
			return QByteArray();
	for(auto i=p->processingBlocks().begin();i!=p->processingBlocks().end();++i)
		if(!blockToXml(f,i.value(),blocksElem))
			return QByteArray();
	QDomElement linksElem=doc.createElement("links");
	root.appendChild(linksElem);
	for(auto i=p->sourceBlocks().begin();i!=p->sourceBlocks().end();++i)
		linksToXml(i.value(),linksElem);
	for(auto i=p->processingBlocks().begin();i!=p->processingBlocks().end();++i)
		linksToXml(i.value(),linksElem);
	return doc.toByteArray();
}

Program* ProgramXmlParser::fromXml(BlocksXmlParserFactory *f,BlocksFactory *bf,const QByteArray &xml)
{
	QDomDocument doc;
	if(!doc.setContent(xml))
		return 0;
	QDomElement rootElem=doc.firstChildElement("GDIL_program");
	if(rootElem.isNull())
		return 0;
	QDomElement blocksElem=rootElem.firstChildElement("blocks");
	QDomElement linksElem=rootElem.firstChildElement("links");
	if(blocksElem.isNull()||linksElem.isNull())
		return 0;
	QScopedPointer<Program> p(new Program);
	//blocks
	for(int i=0;i<blocksElem.childNodes().count();++i)
	{
		QDomElement elem=blocksElem.childNodes().at(i).toElement();
		if(elem.isNull()||elem.nodeName()!="block")
			continue;
		BaseBlock *b=blockFromXml(f,bf,elem);
		if(!b)
			return 0;
		if(!p->addBlock(b))
			return 0;
	}
	//links
	struct LinkDef
	{
		quint32 fromBlockId;
		int fromOutputIndex;
		quint32 toBlockId;
		int toInputIndex;
	};
	QList<LinkDef> linkDefs;
	for(int i=0;i<linksElem.childNodes().count();++i)
	{
		QDomElement elem=linksElem.childNodes().at(i).toElement();
		if(elem.isNull()||elem.nodeName()!="link")continue;
		LinkDef d;
		d.fromBlockId=elem.attribute("from").toUInt();
		d.fromOutputIndex=elem.attribute("from_index").toInt();
		d.toBlockId=elem.attribute("to").toUInt();
		d.toInputIndex=elem.attribute("to_index").toInt();
		linkDefs.append(d);
	}
	bool wasLinks=false;
	do
	{
		for(int i=0;i<linkDefs.count();++i)
		{
			LinkDef &d=linkDefs[i];
			BaseBlock *fromB=p->blockById(d.fromBlockId);
			BaseBlock *toB=p->blockById(d.toBlockId);
			if(!fromB||!toB)
				return 0;
			BlockOutput *from=fromB->output(d.fromOutputIndex);
			BlockInput *to=toB->input(d.toInputIndex);
			if(!from||!to)
				return 0;
			if(!to->canConnectType(from->type(),from->dim()))
				continue;
			if(!from->linkTo(to))continue;
			wasLinks=true;
			linkDefs.removeAt(i);
			--i;
		}
	}
	while(!linkDefs.isEmpty()&&wasLinks);
	if(!wasLinks)
		return 0;
	return p.take();
}

bool ProgramXmlParser::blockToXml(BlocksXmlParserFactory *f,BaseBlock *b,QDomElement &listElem)
{
	IBlockXmlParser *parser=f->parser(b->typeId());
	if(!parser)return false;
	QDomElement blockElem=listElem.ownerDocument().createElement("block");
	listElem.appendChild(blockElem);
	blockElem.setAttribute("id",b->blockId());
	blockElem.setAttribute("type",b->typeId().toString());
	blockElem.setAttribute("title",b->title);
	blockElem.setAttribute("position_x",b->position.x());
	blockElem.setAttribute("position_y",b->position.y());
	parser->blockToXml(b,blockElem);
	return true;
}

void ProgramXmlParser::linksToXml(BaseBlock *b,QDomElement &linksElem)
{
	for(int outIndex=0;outIndex<b->outputsCount();++outIndex)
	{
		BlockOutput *out=b->output(outIndex);
		for(int k=0;k<out->linksCount();++k)
		{
			const BlockInput *in=out->link(k);
			int inIndex=in->block()->inputIndex(in);
			QDomElement linkElem=linksElem.ownerDocument().createElement("link");
			linksElem.appendChild(linkElem);
			linkElem.setAttribute("from",b->blockId());
			linkElem.setAttribute("from_index",outIndex);
			linkElem.setAttribute("to",in->block()->blockId());
			linkElem.setAttribute("to_index",inIndex);
		}
	}
}

BaseBlock* ProgramXmlParser::blockFromXml(BlocksXmlParserFactory *f,BlocksFactory *bf,QDomElement &blockElem)
{
	QUuid typeId(blockElem.attribute("type"));
	if(typeId.isNull())return 0;
	bool ok=false;
	IBlockXmlParser *parser=f->parser(typeId);
	if(!parser)return 0;
	quint32 blockId=blockElem.attribute("id").toUInt(&ok);
	if(!ok)return 0;
	BaseBlock *b=bf->makeBlock(typeId,blockId);
	if(!b)return 0;
	b->title=blockElem.attribute("title");
	b->position.setX(blockElem.attribute("position_x").toInt());
	b->position.setY(blockElem.attribute("position_y").toInt());
	if(!parser->blockFromXml(b,blockElem))
	{
		delete b;
		return 0;
	}
	return b;
}
