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
#include "wliot/WLIOTServerProtocolDefs.h"
#include <QDebug>
#include <QDomDocument>
#include <QDomElement>

using namespace WLIOTClient;

ListTtyCommand::ListTtyCommand(const CmdArgParser &p, ServerConnection *c)
	:IClientCommand(p,c)
{
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
		QByteArray vid=vendorElem.attribute("vid").toUtf8();
		vendors[vid]=vendorElem.attribute("title");
		for(int i=0;i<vendorElem.childNodes().count();++i)
		{
			QDomElement prodElem=vendorElem.childNodes().at(i).toElement();
			if(prodElem.isNull()||prodElem.nodeName()!="product")continue;
			products[vid][prodElem.attribute("pid").toUtf8()]=prodElem.attribute("title");
		}
	}
}

bool ListTtyCommand::evalCommand()
{
	return writeCommandToServer(IClientCommand::listTtyCommand.toUtf8());
}

bool ListTtyCommand::onCmdData(const QByteArrayList &args)
{
	if(args.count()<5)return false;
	QDebug d=StdQFile::inst().stdoutDebug();
	if(args.count()==7)d<<"Identified tty device:";
	else d<<"Tty device:";
	d<<"\n\tport: "<<args[0];
	d<<"\n\tserialNumber: "<<args[1];
	d<<"\n\tmanufacturer: "<<args[2];
	d<<"\n\tusb vendor id (0 - not a usb device): "<<args[3]<<" ("<<vendors[args[3]]<<")";
	d<<"\n\tusb product id (0 - not a usb device): "<<args[4]<<" ("<<products[args[3]][args[4]]<<")\n";
	if(args.count()==7)
	{
		d<<"\tuid="<<args[5];
		d<<"\n\tname="<<args[6]<<"\n";
	}
	d<<"\n";
	return true;
}
