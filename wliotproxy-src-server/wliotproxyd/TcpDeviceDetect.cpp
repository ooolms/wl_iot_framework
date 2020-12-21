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

#include "TcpDeviceDetect.h"
#include "wliot/WLIOTProtocolDefs.h"
#include "MainServerConfig.h"
#include "wliot/devices/StreamParser.h"
#include <QDebug>
#include <QTcpSocket>
#include <QNetworkAddressEntry>
#include <QNetworkInterface>
#include <QUuid>

using namespace WLIOT;

TcpDeviceDetect::TcpDeviceDetect(QObject *parent)
	:QObject(parent)
{
	if(!srv.listen(QHostAddress::AnyIPv4,WLIOTProtocolDefs::netDevicePort))
		qCritical()<<"Can't start listening for tcp devices";
	tmr.setSingleShot(false);
	connect(&srv,&TcpDeviceDetectServer::newClient,this,&TcpDeviceDetect::newClient);
	connect(&tmr,&QTimer::timeout,this,&TcpDeviceDetect::broadcastServerReadyMessage);
}

void TcpDeviceDetect::broadcastServerReadyMessage()
{
	for(QNetworkInterface &i:QNetworkInterface::allInterfaces())
	{
		if(!(i.flags()&QNetworkInterface::CanBroadcast))
			continue;
		for(QNetworkAddressEntry &e:i.addressEntries())
			bCastSock.writeDatagram(bCastMsg,e.broadcast(),WLIOTProtocolDefs::netDevicePort);
	}
	bCastSock.writeDatagram(bCastMsg,QHostAddress::Broadcast,WLIOTProtocolDefs::netDevicePort);
}

bool TcpDeviceDetect::isServerListening()
{
	return srv.isListening();
}

void TcpDeviceDetect::startRegularBroadcasting(int msecsDelay)
{
	Message m;
	m.title=WLIOTProtocolDefs::serverReadyMsg;
	m.args.append(MainServerConfig::serverId.toString().toUtf8());
	m.args.append(MainServerConfig::serverName.toUtf8());
	bCastMsg=m.dump();
	tmr.start(msecsDelay);
}

void TcpDeviceDetect::stopRegularBroadcasting()
{
	tmr.stop();
}
