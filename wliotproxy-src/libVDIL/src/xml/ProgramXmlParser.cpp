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

#include "VDIL/xml/ProgramXmlParser.h"
#include "VDIL/xml/BlocksXmlParserFactory.h"
#include "VDIL/core/BlocksFactory.h"
#include "VDIL/core/ProgramRuntimeVars.h"
#include "VDIL/core/ProgramVirtualDevice.h"
#include "VDIL/xml/DataUnitXmlParser.h"
#include <QDomDocument>
#include <QDomElement>

using namespace WLIOT;
using namespace WLIOTVDIL;

QByteArray ProgramXmlParser::toXml(BlocksXmlParserFactory *f,const Program *p)
{
	QDomDocument doc;
	QDomElement root=doc.createElement("VDIL_program");
	doc.appendChild(root);
	//runtime vars
	QStringList vars=p->runtimeVars()->allVars();
	if(!vars.isEmpty())
	{
		QDomElement varsElem=doc.createElement("runtime_vars");
		root.appendChild(varsElem);
		for(QString &s:vars)
		{
			DataUnit v=p->runtimeVars()->defaultValue(s);
			if(!v.isValid())continue;
			QDomElement varElem=doc.createElement("var");
			varsElem.appendChild(varElem);
			varElem.setAttribute("var_name",s);
			DataUnitXmlParser::toXml(v,varElem);
		}
	}
	//virtual device
	QDomElement vdevElem=doc.createElement("virtual_device");
	root.appendChild(vdevElem);
	vdevElem.setAttribute("enabled",p->vdev()->enabled()?"1":"0");
	vdevElem.setAttribute("device_id",p->vdev()->devId().toString());
	vdevElem.setAttribute("device_name",QString::fromUtf8(p->vdev()->devName()));
	QDomElement ctlElem=doc.createElement("controls");
	vdevElem.appendChild(ctlElem);
	ControlsParser::dumpToXml(ctlElem,p->vdev()->controls());
	QDomElement senseElem=doc.createElement("sensors");
	vdevElem.appendChild(senseElem);
	SensorsParser::dumpToXml(senseElem,p->vdev()->sensors());
	//blocks
	QDomElement blocksElem=doc.createElement("blocks");
	root.appendChild(blocksElem);
	for(auto i=p->allBlocks().begin();i!=p->allBlocks().end();++i)
		if(!blockToXml(f,i.value(),blocksElem))
			return QByteArray();
	//links
	QDomElement linksElem=doc.createElement("links");
	root.appendChild(linksElem);
	for(auto i=p->allBlocks().begin();i!=p->allBlocks().end();++i)
		linksToXml(i.value(),linksElem);
	return doc.toByteArray();
}

Program* ProgramXmlParser::fromXml(BlocksXmlParserFactory *f,BlocksFactory *bf,const QByteArray &xml,bool tryFixErrors)
{
	QDomDocument doc;
	if(!doc.setContent(xml))
		return 0;
	QDomElement rootElem=doc.firstChildElement("VDIL_program");
	if(rootElem.isNull())
		return 0;
	QDomElement blocksElem=rootElem.firstChildElement("blocks");
	QDomElement linksElem=rootElem.firstChildElement("links");
	if(blocksElem.isNull()||linksElem.isNull())
		return 0;
	QScopedPointer<Program> p(new Program);
	//runtime vars
	QDomElement varsElem=rootElem.firstChildElement("runtime_vars");
	if(!varsElem.isNull())
	{
		for(int i=0;i<varsElem.childNodes().count();++i)
		{
			QDomElement varElem=varsElem.childNodes().at(i).toElement();
			if(varElem.isNull()||varElem.nodeName()!="var")continue;
			QString name=varElem.attribute("var_name");
			DataUnit v;
			if(!DataUnitXmlParser::fromXml(v,varElem)||!v.isValid())
			{
				if(tryFixErrors)
					v=DataUnit(0.0);
				else return 0;
			}
			p->runtimeVars()->setupVar(name,v);
		}
	}
	//virtual device
	QDomElement vdevElem=rootElem.firstChildElement("virtual_device");
	if(!vdevElem.isNull())
	{
		bool enabled=vdevElem.attribute("enabled")=="1";
		QUuid devId=QUuid(vdevElem.attribute("device_id"));
		QByteArray devName=vdevElem.attribute("device_name").toUtf8();
		p->vdev()->setParams(enabled,devId,devName);
		QDomElement ctlElem=vdevElem.firstChildElement("controls");
		if(!ctlElem.isNull())
		{
			ControlsGroup g;
			if(!ControlsParser::parseXmlDescription(ctlElem,g)&&!tryFixErrors)
				return 0;
			else p->vdev()->setControls(g);
		}
		else if(!tryFixErrors)return 0;
		QDomElement sensElem=vdevElem.firstChildElement("sensors");
		if(!sensElem.isNull())
		{
			QList<SensorDef> s;
			if(!SensorsParser::parseXmlDescription(sensElem,s)&&!tryFixErrors)
				return 0;
			else p->vdev()->setSensors(s);
		}
		else if(!tryFixErrors)return 0;
	}
	else if(!tryFixErrors)return 0;
	//blocks
	for(int i=0;i<blocksElem.childNodes().count();++i)
	{
		QDomElement elem=blocksElem.childNodes().at(i).toElement();
		if(elem.isNull()||elem.nodeName()!="block")
			continue;
		if(!blockFromXml(p.data(),f,bf,elem,tryFixErrors)&&!tryFixErrors)
			return 0;
	}
	//links - must be stage of parsing
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
	while(1)
	{
		if(linkDefs.isEmpty())break;
		bool wasLinks=false;
		for(int i=0;i<linkDefs.count();++i)
		{
			LinkDef &d=linkDefs[i];
			BaseBlock *fromB=p->blockById(d.fromBlockId);
			BaseBlock *toB=p->blockById(d.toBlockId);
			if(!fromB||!toB)
			{
				if(!tryFixErrors)return 0;
				wasLinks=true;
				linkDefs.removeAt(i);
				--i;
				continue;
			}
			BlockOutput *from=fromB->output(d.fromOutputIndex);
			BlockInput *to=toB->input(d.toInputIndex);
			if(!from||!to)
			{
				if(!tryFixErrors)return 0;
				wasLinks=true;
				linkDefs.removeAt(i);
				--i;
				continue;
			}
			if(!to->supportedTypes().match(from->type(),from->dim()))
				continue;
			if(!from->linkTo(to))
				continue;
			wasLinks=true;
			linkDefs.removeAt(i);
			--i;
		}
		if(!wasLinks)
		{
			if(!tryFixErrors)return 0;
			return p.take();
		}
	}
	return p.take();
}

bool ProgramXmlParser::blockToXml(BlocksXmlParserFactory *f,BaseBlock *b,QDomElement &listElem)
{
	IBlockXmlParser *parser=f->blockXmlParser(b->groupName(),b->blockName());
	if(!parser)return false;
	QDomElement blockElem=listElem.ownerDocument().createElement("block");
	listElem.appendChild(blockElem);
	blockElem.setAttribute("id",b->blockId());
	blockElem.setAttribute("group",b->groupName());
	blockElem.setAttribute("name",b->blockName());
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

bool ProgramXmlParser::blockFromXml(Program *p,
	BlocksXmlParserFactory *f,BlocksFactory *bf,QDomElement &blockElem,bool tryFixErrors)
{
	QString groupName=blockElem.attribute("group");
	QString blockName=blockElem.attribute("name");
	if(groupName.isEmpty()||blockName.isEmpty())
		return false;
	bool ok=false;
	quint32 blockId=blockElem.attribute("id").toUInt(&ok);
	if(!ok)return false;
	IBlockXmlParser *parser=f->blockXmlParser(groupName,blockName);
	if(!parser)return false;
	BaseBlock *b=bf->makeBlock(groupName,blockName,blockId);
	if(!b)return false;
	b->title=blockElem.attribute("title");
	b->position.setX(blockElem.attribute("position_x").toInt());
	b->position.setY(blockElem.attribute("position_y").toInt());
	if(!p->addBlock(b))
	{
		delete b;
		return false;
	}
	if(!parser->blockFromXml(b,blockElem,tryFixErrors))
	{
		p->rmBlock(b->blockId());
		return false;
	}
	return true;
}
