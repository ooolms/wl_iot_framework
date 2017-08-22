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

#include "UdpDataExport.h"
#include <QUuid>

QUdpSocket UdpDataExport::socket;
QHostAddress UdpDataExport::exportAddress;
bool UdpDataExport::enabled=false;
ARpcMessageParser UdpDataExport::parser;
QMutex UdpDataExport::mutex;

void UdpDataExport::setExportAddress(const QString &address)
{
	enabled=!address.isEmpty();
	exportAddress=QHostAddress(address);
}

void UdpDataExport::writeMeasurement(const QUuid &devId,const ARpcMessage &m)
{
	if(!enabled)return;
	QMutexLocker lock(&mutex);
	ARpcMessage msg(devId.toString(),m.args);
	socket.writeDatagram((parser.dump(msg)+ARpcConfig::msgDelim).toUtf8(),exportAddress,exportPort);
}

