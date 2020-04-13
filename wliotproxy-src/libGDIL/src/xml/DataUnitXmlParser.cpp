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

#include "GDIL/xml/DataUnitXmlParser.h"
#include "wliot/devices/StreamParser.h"

static QString typeToStr(DataUnit::Type t)
{
	if(t==DataUnit::SINGLE)
		return "s";
	else if(t==DataUnit::ARRAY)
		return "a";
	else if(t==DataUnit::BOOL)
		return "b";
	return "";
}

static DataUnit::Type typeFromStr(const QString &str)
{
	if(str=="s")
		return DataUnit::SINGLE;
	else if(str=="a")
		return DataUnit::ARRAY;
	else if(str=="b")
		return DataUnit::BOOL;
	return DataUnit::INVALID;
}

static QString numTypeToStr(DataUnit::NumericType t)
{
	if(t==DataUnit::F64)
		return "f64";
	else if(t==DataUnit::S64)
		return "s64";
	return "";
}

static DataUnit::NumericType numTypeFromStr(const QString &str)
{
	if(str=="s64")
		return DataUnit::S64;
	return DataUnit::F64;
}

void DataUnitXmlParser::toXml(const DataUnit &u,QDomElement &elem)
{
	elem.setAttribute("type",typeToStr(u.type()));
	elem.setAttribute("num_type",numTypeToStr(u.numType()));
	QByteArray args=Message::dump("",u.value()->dumpToMsgArgs()).mid(1);
	args.chop(1);
	elem.setAttribute("value",QString::fromUtf8(args));
	elem.setAttribute("dim",u.dim());
}

bool DataUnitXmlParser::fromXml(DataUnit &u,const QDomElement &elem)
{
	if(!elem.hasAttribute("type")||!elem.hasAttribute("num_type")||
		!elem.hasAttribute("value")||!elem.hasAttribute("dim"))
		return false;
	DataUnit::NumericType nt=numTypeFromStr(elem.attribute("num_type"));
	DataUnit::Type t=typeFromStr(elem.attribute("type"));
	if(t==DataUnit::INVALID)
		return false;
	bool ok=false;
	quint32 dim=elem.attribute("dim").toUInt(&ok);
	DataUnit u2(t,dim,nt);
	Message m;
	if(!StreamParser::tryParse(WLIOTProtocolDefs::argDelim+elem.attribute("value").toUtf8()+
		WLIOTProtocolDefs::msgDelim,m))
			return false;
	if(!u2.valueFromArgs(m.args))
		return false;
	u=u2;
	return true;
}
