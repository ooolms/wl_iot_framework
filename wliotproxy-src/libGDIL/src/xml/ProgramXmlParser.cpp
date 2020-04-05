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
	QDomElement sourceElem=doc.createElement("source_blocks");
	root.appendChild(sourceElem);
	for(auto i=p->sourceBlocks().begin();i!=p->sourceBlocks().end();++i)
		if(!blockToXml(f,i.value(),sourceElem))
			return QByteArray();
	QDomElement processingElem=doc.createElement("processing_blocks");
	root.appendChild(processingElem);
	for(auto i=p->processingBlocks().begin();i!=p->processingBlocks().end();++i)
		if(!blockToXml(f,i.value(),processingElem))
			return QByteArray();
	QDomElement linksElem=doc.createElement("links");
	root.appendChild(linksElem);
	for(auto i=p->sourceBlocks().begin();i!=p->sourceBlocks().end();++i)
		linksToXml(i.value(),linksElem);
	for(auto i=p->processingBlocks().begin();i!=p->processingBlocks().end();++i)
		linksToXml(i.value(),linksElem);
	QDomElement devTriggersElem=doc.createElement("device_triggers");
	root.appendChild(devTriggersElem);
	QDomElement storageTriggersElem=doc.createElement("storage_triggers");
	root.appendChild(storageTriggersElem);
	for(const QUuid &devId:p->deviceTriggers)
	{
		QDomElement elem=doc.createElement("device");
		devTriggersElem.appendChild(elem);
		elem.setAttribute("dev_id",devId.toString());
	}
	for(const StorageId &stId:p->storageTriggers)
	{
		QDomElement elem=doc.createElement("storage");
		devTriggersElem.appendChild(elem);
		elem.setAttribute("dev_id",stId.deviceId.toString());
		elem.setAttribute("sensor_name",QString::fromUtf8(stId.sensorName));
	}
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
	QDomElement sourceElem=rootElem.firstChildElement("source_blocks");
	QDomElement processingElem=rootElem.firstChildElement("processing_blocks");
	QDomElement linksElem=rootElem.firstChildElement("links");
	QDomElement devTriggersElem=rootElem.firstChildElement("device_triggers");
	QDomElement storageTriggersElem=rootElem.firstChildElement("storage_triggers");
	if(sourceElem.isNull()||processingElem.isNull()||linksElem.isNull()||
		devTriggersElem.isNull()||storageTriggersElem.isNull())
		return 0;
	QScopedPointer<Program> p(new Program);
	//source blocks
	for(int i=0;i<sourceElem.childNodes().count();++i)
	{
		QDomElement elem=sourceElem.childNodes().at(i).toElement();
		if(elem.isNull()||elem.nodeName()!="block")
			continue;
		BaseBlock *b=blockFromXml(f,bf,elem);
		if(!b)
			return 0;
		if(!p->addSourceBlock((SourceBlock*)b))
			return 0;
	}
	//processing blocks
	for(int i=0;i<processingElem.childNodes().count();++i)
	{
		QDomElement elem=processingElem.childNodes().at(i).toElement();
		if(elem.isNull()||elem.nodeName()!="block")
			continue;
		BaseBlock *b=blockFromXml(f,bf,elem);
		if(!b)
			return 0;
		if(!p->addProcessingBlock((SourceBlock*)b))
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
	for(int i=0;i<devTriggersElem.childNodes().count();++i)
	{
		QDomElement elem=devTriggersElem.childNodes().at(i).toElement();
		if(elem.isNull()||elem.nodeName()!="device")
			continue;
		QUuid devId(elem.attribute("dev_id"));
		if(!devId.isNull())
			p->deviceTriggers.append(devId);
	}
	for(int i=0;i<storageTriggersElem.childNodes().count();++i)
	{
		QDomElement elem=storageTriggersElem.childNodes().at(i).toElement();
		if(elem.isNull()||elem.nodeName()!="storage")
			continue;
		QUuid devId(elem.attribute("dev_id"));
		QByteArray sensorName=elem.attribute("sensor_name").toUtf8();
		if(!devId.isNull()&&!sensorName.isEmpty())
			p->storageTriggers.append({devId,sensorName});
	}
	return p.take();
}

bool ProgramXmlParser::blockToXml(BlocksXmlParserFactory *f,BaseBlock *b,QDomElement &listElem)
{
	IBlockXmlParser *parser=f->parser(b->typeId());
	if(!parser)return false;
	QDomElement blockElem=listElem.ownerDocument().createElement("block");
	listElem.appendChild(blockElem);
	blockElem.setAttribute("id",b->mBlockId);
	blockElem.setAttribute("type",b->typeId().toString());
	blockElem.setAttribute("title",b->title);
	blockElem.setAttribute("position_x",b->position.x());
	blockElem.setAttribute("position_y",b->position.y());
	parser->blockToXml(b,blockElem);
	return true;
}

void ProgramXmlParser::linksToXml(BaseBlock *b,QDomElement &linksElem)
{
	for(int j=0;j<b->outputsCount();++j)
	{
		BlockOutput *out=b->output(j);
		for(int k=0;k<out->linksCount();++k)
		{
			const BlockInput *in=out->link(k);
			QDomElement linkElem=linksElem.ownerDocument().createElement("link");
			linksElem.appendChild(linkElem);
			linkElem.setAttribute("from",b->mBlockId);
			linkElem.setAttribute("from_index",k);
			linkElem.setAttribute("to",in->block()->mBlockId);
			linkElem.setAttribute("to_index",in->block()->inputIndex(in));
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
