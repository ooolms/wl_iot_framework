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
#include "VDIL/core/CoreBlocksGroupFactory.h"
#include "VDIL/core/SubProgramBlock.h"
#include "VDIL/core/SubProgram.h"
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
	for(auto i=p->selfBlocks().begin();i!=p->selfBlocks().end();++i)
		if(!blockToXml(f,i.value(),blocksElem))
			return QByteArray();
	//links
	QDomElement linksElem=doc.createElement("links");
	root.appendChild(linksElem);
	for(BaseBlock *b:p->selfBlocks())
	{
		for(int inIndex=0;inIndex<b->inputsCount();++inIndex)
		{
			BlockInput *in=b->input(inIndex);
			if(!in->linkedOutput())continue;
			BlockOutput *out=in->linkedOutput();
			int outIndex=out->block()->outputIndex(out);
			QDomElement linkElem=linksElem.ownerDocument().createElement("link");
			linksElem.appendChild(linkElem);
			linkElem.setAttribute("from",out->block()->blockId());
			linkElem.setAttribute("from_index",outIndex);
			linkElem.setAttribute("to",in->block()->blockId());
			linkElem.setAttribute("to_index",inIndex);
		}

	}
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
		if(!blockFromXml(p.data(),f,bf,elem,tryFixErrors))
			return 0;
	}
	//links - must be last stage of parsing
	/*QList<LinkDef> linkDefs;
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
	}*/
	if(!renderLinks(p.data(),0,linksElem,tryFixErrors))
		return 0;
	return p.take();
}

bool ProgramXmlParser::blockToXml(BlocksXmlParserFactory *f,BaseBlock *b,QDomElement &listElem)
{
	IBlockXmlParser *parser=0;
	bool isSubPRogramBlock=b->groupName()==CoreBlocksGroupFactory::mGroupName&&
		b->blockName()==SubProgramBlock::mBlockName;
	if(!isSubPRogramBlock)
	{
		parser=f->blockXmlParser(b->groupName(),b->blockName());
		if(!parser)return false;
	}
	QDomElement blockElem=listElem.ownerDocument().createElement("block");
	listElem.appendChild(blockElem);
	blockElem.setAttribute("id",b->blockId());
	blockElem.setAttribute("group",b->groupName());
	blockElem.setAttribute("name",b->blockName());
	blockElem.setAttribute("title",b->title);
	blockElem.setAttribute("position_x",b->position.x());
	blockElem.setAttribute("position_y",b->position.y());
	if(isSubPRogramBlock)
		subProgramToXml(f,(SubProgramBlock*)b,blockElem);
	else parser->blockToXml(b,blockElem);
	return true;
}

bool ProgramXmlParser::blockFromXml(SubProgram *p,
	BlocksXmlParserFactory *f,BlocksFactory *bf,QDomElement &blockElem,bool tryFixErrors)
{
	QString groupName=blockElem.attribute("group");
	QString blockName=blockElem.attribute("name");
	if(groupName.isEmpty()||blockName.isEmpty())
		return false;
	bool ok=false;
	quint32 blockId=blockElem.attribute("id").toUInt(&ok);
	if(!ok)return false;
	bool isSubPRogramBlock=groupName==CoreBlocksGroupFactory::mGroupName&&
		blockName==SubProgramBlock::mBlockName;
	IBlockXmlParser *parser=0;
	if(!isSubPRogramBlock)
	{
		parser=f->blockXmlParser(groupName,blockName);
		if(!parser)return false;
	}
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
	if(!isSubPRogramBlock)
	{
		if(!parser->blockFromXml(b,blockElem,tryFixErrors))
		{
			p->rmBlock(b->blockId());
			return false;
		}
	}
	else if(!subProgramFromXml(f,bf,(SubProgramBlock*)b,blockElem,tryFixErrors))
	{
		p->rmBlock(b->blockId());
		return false;
	}
	return true;
}

void ProgramXmlParser::subProgramToXml(BlocksXmlParserFactory *f,SubProgramBlock *b,QDomElement &blockElem)
{
	QDomDocument doc=blockElem.ownerDocument();
	//common attributes and ext. inputs/outputs
	blockElem.setAttribute("input_block_x",b->internalOutputsBlock()->position.x());
	blockElem.setAttribute("input_block_y",b->internalOutputsBlock()->position.y());
	blockElem.setAttribute("output_block_x",b->internalInputsBlock()->position.x());
	blockElem.setAttribute("output_block_y",b->internalInputsBlock()->position.y());
	QDomElement inputsElem=doc.createElement("inputs");
	blockElem.appendChild(inputsElem);
	QDomElement outputsElem=doc.createElement("outputs");
	blockElem.appendChild(outputsElem);
	for(int i=0;i<b->argInputsCount();++i)
	{
		BlockInput *in=b->argInput(i);
		QDomElement elem=doc.createElement("input");
		inputsElem.appendChild(elem);
		elem.setAttribute("type",QString::fromUtf8(DataUnit::typeToStr(in->type().type)));
		elem.setAttribute("dim",in->type().dim);
		elem.setAttribute("title",in->title());
	}
	for(int i=0;i<b->argOutputsCount();++i)
	{
		BlockOutput *out=b->argOutput(i);
		QDomElement elem=doc.createElement("output");
		outputsElem.appendChild(elem);
		elem.setAttribute("type",QString::fromUtf8(DataUnit::typeToStr(out->type().type)));
		elem.setAttribute("dim",out->type().dim);
		elem.setAttribute("title",out->title());
	}
	//internal blocks
	QDomElement blocksElem=doc.createElement("blocks");
	blockElem.appendChild(blocksElem);
	for(BaseBlock *cb:b->subProgram()->selfBlocks())
		blockToXml(f,cb,blocksElem);
	//internal links
	QDomElement linksElem=doc.createElement("links");
	blockElem.appendChild(linksElem);
	QList<BaseBlock*> sprgBlocks=b->subProgram()->selfBlocks().values();
	sprgBlocks.append(b->internalInputsBlock());
	for(BaseBlock *cb:sprgBlocks)
	{
		for(int i=0;i<cb->inputsCount();++i)
		{
			BlockInput *in=cb->input(i);
			if(!in->linkedOutput())continue;
			BlockOutput *out=in->linkedOutput();
			QDomElement elem=doc.createElement("link");
			linksElem.appendChild(elem);
			elem.setAttribute("from",out->block()->blockId());
			elem.setAttribute("from_index",out->block()->outputIndex(out));
			elem.setAttribute("to",cb->blockId());
			elem.setAttribute("to_index",i);
		}
	}
}

bool ProgramXmlParser::subProgramFromXml(BlocksXmlParserFactory *f,
	BlocksFactory *bf,SubProgramBlock *b,QDomElement &blockElem,bool tryFixErrors)
{
	//common attributes and ext. inputs/outputs
	QDomElement inputsElem=blockElem.firstChildElement("inputs");
	QDomElement outputsElem=blockElem.firstChildElement("outputs");
	QList<TypeAndDim> inputsTypes,outputsTypes;
	QStringList inputsTitles,outputsTitles;
	b->internalOutputsBlock()->position=QPointF(blockElem.attribute("input_block_x").toDouble(),
		blockElem.attribute("input_block_y").toDouble());
	b->internalInputsBlock()->position=QPointF(blockElem.attribute("output_block_x").toDouble(),
		blockElem.attribute("output_block_y").toDouble());
	for(int i=0;i<inputsElem.childNodes().count();++i)
	{
		QDomElement elem=inputsElem.childNodes().at(i).toElement();
		if(elem.isNull()||elem.nodeName()!="input")continue;
		TypeAndDim t;
		t.type=DataUnit::typeFromStr(elem.attribute("type").toUtf8());
		t.dim=elem.attribute("dim").toUInt();
		if(t.type==DataUnit::INVALID)
		{
			if(tryFixErrors)
				t.type=DataUnit::SINGLE;
			else return false;
		}
		if(t.dim==0)
		{
			if(tryFixErrors)
				t.dim=1;
			else return false;
		}
		inputsTypes.append(t);
		inputsTitles.append(elem.attribute("title"));
	}
	for(int i=0;i<outputsElem.childNodes().count();++i)
	{
		QDomElement elem=outputsElem.childNodes().at(i).toElement();
		if(elem.isNull()||elem.nodeName()!="output")continue;
		TypeAndDim t;
		t.type=DataUnit::typeFromStr(elem.attribute("type").toUtf8());
		t.dim=elem.attribute("dim").toUInt();
		if(t.type==DataUnit::INVALID)
		{
			if(tryFixErrors)
				t.type=DataUnit::SINGLE;
			else return false;
		}
		if(t.dim==0)
		{
			if(tryFixErrors)
				t.dim=1;
			else return false;
		}
		outputsTypes.append(t);
		outputsTitles.append(elem.attribute("title"));
	}
	b->setParams(inputsTypes,inputsTitles,outputsTypes,outputsTitles);
	//internal blocks
	QDomElement blocksElem=blockElem.firstChildElement("blocks");
	for(int i=0;i<blocksElem.childNodes().count();++i)
	{
		QDomElement elem=blocksElem.childNodes().at(i).toElement();
		if(elem.isNull()||elem.nodeName()!="block")
			continue;
		if(!blockFromXml(b->subProgram(),f,bf,elem,tryFixErrors))
			return 0;
	}
	//internal links
	return renderLinks(b->subProgram(),b,blockElem.firstChildElement("links"),tryFixErrors);
}

bool ProgramXmlParser::renderLinks(SubProgram *p,SubProgramBlock *b,
	QDomElement linksElem,bool tryFixErrors)
{
	QList<LinkDef> links;
	for(int i=0;i<linksElem.childNodes().count();++i)
	{
		QDomElement elem=linksElem.childNodes().at(i).toElement();
		if(elem.isNull()||elem.nodeName()!="link")continue;
		LinkDef d;
		d.fromBlockId=elem.attribute("from","0").toUInt();
		d.fromOutputIndex=elem.attribute("from_index").toInt();
		d.toBlockId=elem.attribute("to","0").toUInt();
		d.toInputIndex=elem.attribute("to_index").toInt();
		links.append(d);
	}
	while(1)
	{
		if(links.isEmpty())break;
		bool wasLinks=false;
		for(int i=0;i<links.count();++i)
		{
			LinkDef &d=links[i];
			BlockOutput *from=0;
			BlockInput *to=0;
			if(d.fromBlockId==0)
			{
				if(b)from=b->internalOutputsBlock()->output(d.fromOutputIndex);
			}
			else
			{
				BaseBlock *b=p->selfBlocks().value(d.fromBlockId);
				if(b)from=b->output(d.fromOutputIndex);
			}
			if(d.toBlockId==0)
			{
				if(b)to=b->internalInputsBlock()->input(d.toInputIndex);
			}
			else
			{
				BaseBlock *b=p->selfBlocks().value(d.toBlockId);
				if(b)to=b->input(d.toInputIndex);
			}
			if(!from||!to)
			{
				if(!tryFixErrors)return 0;
				wasLinks=true;
				links.removeAt(i);
				--i;
				continue;
			}
			if(!from->linkTo(to))
				continue;
			wasLinks=true;
			links.removeAt(i);
			--i;
		}
		if(!wasLinks)
		{
			if(!tryFixErrors)return false;
			return true;
		}
	}
	return true;
}
