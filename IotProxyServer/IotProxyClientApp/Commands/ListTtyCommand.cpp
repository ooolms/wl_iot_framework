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

#include "ListTtyCommand.h"
#include "../StdQFile.h"
#include "../ShowHelp.h"
#include <QDebug>
#include <QDomDocument>
#include <QDomElement>

ListTtyCommand::ListTtyCommand(const CmdArgParser &p,ARpcOutsideDevice *d)
	:IClientCommand(p,d)
{
	connect(dev,&ARpcOutsideDevice::rawMessage,this,&ListTtyCommand::onRawMessage);
	QFile file(":/client/USB_VIDS_PIDS.xml");
	file.open(QIODevice::ReadOnly);
	QDomDocument doc;
	doc.setContent(file.readAll());
	file.close();
	QDomElement rootElem=doc.firstChildElement("usb-vendors");
	if(rootElem.isNull())return;
	for(int i=0;i<rootElem.childNodes().count();++i)
	{
		QDomElement vendorElem=rootElem.childNodes().at(i).toElement();
		if(vendorElem.isNull()||vendorElem.nodeName()!="vendor")continue;
		QString vid=vendorElem.attribute("vid");
		vendors[vid]=vendorElem.attribute("title");
		for(int i=0;i<vendorElem.childNodes().count();++i)
		{
			QDomElement prodElem=vendorElem.childNodes().at(i).toElement();
			if(prodElem.isNull()||prodElem.nodeName()!="product")continue;
			products[vid][prodElem.attribute("pid")]=prodElem.attribute("title");
		}
	}
}

bool ListTtyCommand::evalCommand()
{
	return dev->writeMsg(IClientCommand::listTtyCommand);
}

void ListTtyCommand::onRawMessage(const ARpcMessage &m)
{
	if(m.title==ARpcConfig::srvCmdDataMsg)
	{
		if(m.args.count()<5)return;
		QDebug d=StdQFile::inst().stdoutDebug();
		if(m.args.count()==7)d<<"Identified tty device:";
		else d<<"Tty device:";
		d<<"\n\tport: "<<m.args[0];
		d<<"\n\tserialNumber: "<<m.args[1];
		d<<"\n\tmanufacturer: "<<m.args[2];
		d<<"\n\tusb vendor id (0 - not a usb device): "<<m.args[3]<<" ("<<vendors[m.args[3]]<<")";
		d<<"\n\tusb product id (0 - not a usb device): "<<m.args[4]<<" ("<<products[m.args[3]][m.args[4]]<<")\n";
		if(m.args.count()==7)
		{
			d<<"\tuid="<<m.args[5];
			d<<"\n\tname="<<m.args[6]<<"\n";
		}
		d<<"\n";
	}
}
