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

#include "ARpcTcpDeviceDetect.h"
#include "ARpcBase/ARpcConfig.h"
#include "IotProxyConfig.h"
#include "ARpcBase/ARpcStreamParser.h"
#include <QDebug>
#include <QTcpSocket>
#include <QUuid>

ARpcTcpDeviceDetect::ARpcTcpDeviceDetect(QObject *parent)
	:QObject(parent)
{
	if(!srv.listen(QHostAddress::AnyIPv4,ARpcConfig::netDevicePort))
		qDebug()<<"Can't start listening for tcp devices";
	tmr.setSingleShot(false);
	connect(&srv,&ARpcTcpDeviceDetectServer::newClient,this,&ARpcTcpDeviceDetect::newClient);
	connect(&tmr,&QTimer::timeout,this,&ARpcTcpDeviceDetect::broadcastServerReadyMessage);
}

void ARpcTcpDeviceDetect::broadcastServerReadyMessage()
{
	bCastSock.writeDatagram(bCastMsg,QHostAddress("255.255.255.255"),ARpcConfig::netDevicePort);
}

bool ARpcTcpDeviceDetect::isServerListening()
{
	return srv.isListening();
}

void ARpcTcpDeviceDetect::startRegularBroadcasting(int msecsDelay)
{
	ARpcMessage m;
	m.title=ARpcConfig::serverReadyMsg;
	m.args.append(IotProxyConfig::serverId.toString().toUtf8());
	m.args.append(IotProxyConfig::serverName.toUtf8());
	bCastMsg=ARpcStreamParser::dump(m);
	tmr.start(msecsDelay);
}

void ARpcTcpDeviceDetect::stopRegularBroadcasting()
{
	tmr.stop();
}
