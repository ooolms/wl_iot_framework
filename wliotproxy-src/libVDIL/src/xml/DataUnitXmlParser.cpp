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

#include "VDIL/xml/DataUnitXmlParser.h"
#include "wliot/devices/StreamParser.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

void DataUnitXmlParser::toXml(const DataUnit &u,QDomElement &elem)
{
	elem.setAttribute("type",QString::fromUtf8(DataUnit::typeToStr(u.type())));
	QByteArray args=Message::dump("",u.value()->dumpToMsgArgs()).mid(1);
	args.chop(1);
	elem.setAttribute("value",QString::fromUtf8(args));
	elem.setAttribute("dim",u.dim());
}

bool DataUnitXmlParser::fromXml(DataUnit &u,const QDomElement &elem)
{
	if(!elem.hasAttribute("type")||!elem.hasAttribute("value")||!elem.hasAttribute("dim"))
		return false;
	DataUnit::Type t=DataUnit::typeFromStr(elem.attribute("type").toUtf8());
	if(t==DataUnit::INVALID)
		return false;
	bool ok=false;
	quint32 dim=elem.attribute("dim").toUInt(&ok);
	if(!ok)return false;
	Message m;
	if(!StreamParser::tryParse(WLIOTProtocolDefs::argDelim+elem.attribute("value").toUtf8()+
		WLIOTProtocolDefs::msgDelim,m))
			return false;
	DataUnit u2(t,dim,m.args);
	if(!u2.isValid())
		return false;
	u=u2;
	return true;
}
