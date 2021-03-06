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
#include "wliot/devices/StreamParser.h"
#include <QUuid>

QUdpSocket UdpDataExport::socket;
QHostAddress UdpDataExport::exportAddress;
bool UdpDataExport::enabled=false;
QMutex UdpDataExport::mutex;

using namespace WLIOT;

void UdpDataExport::setExportAddress(const QString &address)
{
	enabled=!address.isEmpty();
	exportAddress=QHostAddress(address);
}

void UdpDataExport::writeMeasurement(const QUuid &devId,const QByteArray &sensorName,const QByteArrayList &data)
{
	if(!enabled)return;
	QMutexLocker lock(&mutex);
	Message msg(devId.toByteArray(),QByteArrayList()<<sensorName<<data);
	socket.writeDatagram(msg.dump(),exportAddress,exportPort);
}

