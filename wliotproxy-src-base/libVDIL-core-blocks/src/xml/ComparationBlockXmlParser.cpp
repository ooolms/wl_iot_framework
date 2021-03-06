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

#include "VDIL/xml/ComparationBlockXmlParser.h"
#include "VDIL/blocks/ComparationBlock.h"
#include "VDIL/xml/DataUnitXmlParser.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

static QString opToStr(ComparationBlock::Operation op)
{
	if(op==ComparationBlock::EQ)
		return QString::fromUtf8("==");
	else if(op==ComparationBlock::NEQ)
		return QString::fromUtf8("!=");
	else if(op==ComparationBlock::LT)
		return QString::fromUtf8("<");
	else if(op==ComparationBlock::GT)
		return QString::fromUtf8(">");
	else if(op==ComparationBlock::LTEQ)
		return QString::fromUtf8("<=");
	else if(op==ComparationBlock::GTEQ)
		return QString::fromUtf8(">=");
	else if(op==ComparationBlock::DISTANCE)
		return QString::fromUtf8("dist");
	else return QString();
}

static ComparationBlock::Operation opFromStr(const QString &s)
{
	if(s==QString::fromUtf8("!="))
		return ComparationBlock::NEQ;
	else if(s==QString::fromUtf8("<"))
		return ComparationBlock::LT;
	else if(s==QString::fromUtf8(">"))
		return ComparationBlock::GT;
	else if(s==QString::fromUtf8("<="))
		return ComparationBlock::LTEQ;
	else if(s==QString::fromUtf8(">="))
		return ComparationBlock::GTEQ;
	else if(s==QString::fromUtf8("dist"))
		return ComparationBlock::DISTANCE;
	else return ComparationBlock::EQ;
}


static QString outModeToStr(ComparationBlock::OutMode md)
{
	if(md==ComparationBlock::SPLITTED_BOOL)
		return QString::fromUtf8("2bool");
	else if(md==ComparationBlock::SPLITTED_INPUT)
		return QString::fromUtf8("v1");
	else return QString::fromUtf8("bool");
}

static ComparationBlock::OutMode outModeFromStr(const QString &s)
{
	if(s==QString::fromUtf8("2bool"))
		return ComparationBlock::SPLITTED_BOOL;
	else if(s==QString::fromUtf8("v1"))
		return ComparationBlock::SPLITTED_INPUT;
	else return ComparationBlock::SINGLE_BOOL;
}

bool ComparationBlockXmlParser::blockFromXml(BaseBlock *block,const QDomElement &blockElem,bool tryFixErrors)
{
	ComparationBlock *b=(ComparationBlock*)block;
	QDomElement distValElem=blockElem.firstChildElement("dist_value");
	QDomElement v2ValElem=blockElem.firstChildElement("v2_value");
	if(!blockElem.hasAttribute("out_mode")||!blockElem.hasAttribute("operation")||
		!blockElem.hasAttribute("external_v2_input")||!blockElem.hasAttribute("dim_index")||
		distValElem.isNull()||v2ValElem.isNull())
			if(!tryFixErrors)return false;
	ComparationBlock::OutMode outMode=outModeFromStr(blockElem.attribute("out_mode"));
	ComparationBlock::Operation op=opFromStr(blockElem.attribute("operation"));
	bool extV2=blockElem.attribute("external_v2_input")!="0";
	bool ok=false;
	quint32 dimIndex=blockElem.attribute("dim_index").toUInt(&ok);
	if(!ok&&!tryFixErrors)return false;
	DataUnit distValue,v2Value;
	DataUnitXmlParser::fromXml(distValue,distValElem);
	DataUnitXmlParser::fromXml(v2Value,v2ValElem);
	if(!distValue.isValid())
		distValue=DataUnit(1.0);
	if(!v2Value.isValid())
		distValue=DataUnit(0.0);
	b->setParams(outMode,extV2,dimIndex,op);
	b->setDistValue(distValue);
	b->setV2Value(v2Value);
	return true;
}

void ComparationBlockXmlParser::blockToXml(const BaseBlock *block,QDomElement &blockElem)
{
	const ComparationBlock *b=(const ComparationBlock*)block;
	blockElem.setAttribute("out_mode",outModeToStr(b->outMode()));
	blockElem.setAttribute("operation",opToStr(b->operation()));
	blockElem.setAttribute("dim_index",b->dimIndex());
	blockElem.setAttribute("external_v2_input",QString::fromUtf8(b->externalV2Input()?"1":"0"));
	blockElem.setAttribute("dim_index",b->dimIndex());
	QDomElement distValElem=blockElem.ownerDocument().createElement("dist_value");
	blockElem.appendChild(distValElem);
	DataUnitXmlParser::toXml(b->distValue(),distValElem);
	QDomElement v2ValElem=blockElem.ownerDocument().createElement("v2_value");
	blockElem.appendChild(v2ValElem);
	DataUnitXmlParser::toXml(b->v2Value(),v2ValElem);
}
